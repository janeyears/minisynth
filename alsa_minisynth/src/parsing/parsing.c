#include "minisynth.h"

static t_inst parse_instrument(const char *name) {
	if (strcmp(name, "sine") == 0) 
		return SINE;
	if (strcmp(name, "saw") == 0) 
		return SAW;
	if (strcmp(name, "square") == 0) 
		return SQUARE;
	if (strcmp(name, "triangle") == 0) 
		return TRIANGLE;
	if (strcmp(name, "kick") == 0) 
		return KICK;
	if (strcmp(name, "snare") == 0) 
		return SNARE;
	return SINE;
}

static t_note parse_note_token(const char *token, int *prev_octave, double *prev_duration) 
{
	t_note n;
	n.pitch = token[0];
	n.alteration = '\0';
	n.octave = *prev_octave;
	n.duration = *prev_duration;

	int i = 1;
	if (token[i] == '#' || token[i] == 'b')
	{
		n.alteration = token[i]; 
		i++; 
	}
	if (isdigit((unsigned char)token[i])) { 
		n.octave = token[i] - '0';   // convert char '4' → int 4
		*prev_octave = n.octave;     // store for next note
		i++; 
	}
	if (token[i] == '/')
	{
		n.duration = atof(token + i + 1);
		*prev_duration = n.duration;
	}
	return n;
}

static void add_note(t_track *track, t_note n) 
{
	if (track->note_count >= track->note_capacity) 
	{
		track->note_capacity = (track->note_capacity == 0) ? 8 : track->note_capacity * 2;
		track->notes = realloc(track->notes, track->note_capacity * sizeof(t_note));
		if (!track->notes) 
		{ 
			perror("realloc"); 
			exit(1); 
		}
	}
	track->notes[track->note_count++] = n;
}

static t_list *read_file_to_list(const char *filename) 
{
	FILE *fd = fopen(filename, "r");
	if (!fd) {
		perror("Can't open the file");
		exit(1);
	}

	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t linelen;
	t_list *head = NULL;
	t_list *tail = NULL;

	while ((linelen = getline(&buffer, &bufsize, fd)) != -1) 
	{
		char *line = buffer;
		if (*line == '\0' || *line == '#') 
			continue;
		t_list *node = malloc(sizeof(t_list));
		if (!node) {
			perror("malloc");
			exit(1);
		}
		node->line = strdup(line);
		if (!node->line) {
			perror("strdup");
			exit(1);
		}
		node->next = NULL;
		if (!head) 
			head = node;
		else 
			tail->next = node;
		tail = node;
	}
	free(buffer);
	fclose(fd);
	return head;
}

static void parse_tempo(char *line, t_song *song, int *tempo_found) {
	if (!(*tempo_found) && strncmp(line, "tempo", 5) == 0) {
		*tempo_found = 1;
		song->tempo = atoi(line + 5);
	}
}

static void parse_tracks(char *line, t_song *song, int *tracks_found) {
	if (!(*tracks_found) && strncmp(line, "tracks", 6) == 0) {
		*tracks_found = 1;
		char *instr_list = trim(line + 6);

		int track_count = 1;
		for (char *p = instr_list; *p; p++)
			if (*p == ',') track_count++;

		song->track_count = track_count;
		song->tracks = calloc(track_count, sizeof(t_track));

		int i = 0;
		char *token = strtok(instr_list, ",");
		while (token) {
			song->tracks[i].instrument = parse_instrument(trim(token));
			song->tracks[i].notes = NULL;
			song->tracks[i].note_count = 0;
			song->tracks[i].note_capacity = 0;
			song->tracks[i].volume = 100;   // default
			song->tracks[i].attack  = 0.01; // ADSR defaults
			song->tracks[i].decay   = 0.1;
			song->tracks[i].sustain = 0.8;
			song->tracks[i].release = 0.3;
			i++;
			token = strtok(NULL, ",");
		}
	}
}

static void parse_volumes(char *line, t_song *song) {
	if (strncmp(line, "volumes", 7) == 0) {
		char *vol_str = trim(line + 7);
		int i = 0;
		char *token = strtok(vol_str, ",");
		while (token && i < song->track_count) {
			song->tracks[i].volume = atof(trim(token));
			i++;
			token = strtok(NULL, ",");
		}
	}
}

static void parse_adsr(char *line, t_song *song) {
	if (strncmp(line, "adsr", 4) == 0) {
		int track_num = 0;
		double a = 0.0, d = 0.0, s = 0.0, r = 0.0;

		if (sscanf(line, "adsr track %d %lf %lf %lf %lf",
				&track_num, &a, &d, &s, &r) != 5) {
			fprintf(stderr, "Invalid ADSR line: %s\n", line);
			exit(1);
		}

		track_num -= 1; // adjust to 0-based index
		if (track_num < 0 || track_num >= song->track_count) {
			fprintf(stderr, "ADSR track number out of range: %d\n", track_num + 1);
			exit(1);
		}

		song->tracks[track_num].attack  = a;
		song->tracks[track_num].decay   = d;
		song->tracks[track_num].sustain = s;
		song->tracks[track_num].release = r;
	}
}

static void parse_track_notes(char *line, t_song *song) {
	if (isdigit((unsigned char)line[0])) {
		char *colon = strchr(line, ':');
		*colon = '\0';
		int track_num = atoi(line) - 1;
		char *notes_str = trim(colon + 1);
		int prev_octave = 4;
		double prev_duration = 1.0;
		char prev_pitch = 'c';

		char *note_token = strtok(notes_str, " \t|");
		while (note_token) {
			t_note n = parse_note_token(note_token, &prev_octave, &prev_duration);
			if (is_note_letter(n.pitch) || n.pitch == 'r')
				prev_pitch = n.pitch;
			else
				n.pitch = prev_pitch;
			add_note(&song->tracks[track_num], n);
			note_token = strtok(NULL, " \t|");
		}
	}
}

static void parse_linked_list(t_list *head, t_song *song) {
	t_list *curr = head;
	int tempo_found = 0;
	int tracks_found = 0;

	while (curr) {
		char *line = curr->line;

		parse_tempo(line, song, &tempo_found);
		parse_tracks(line, song, &tracks_found);
		parse_volumes(line, song);
		parse_adsr(line, song);
		parse_track_notes(line, song);

		curr = curr->next;
	}
}

void parsing(char *file, t_song *song) {
	t_list *lines = read_file_to_list(file);
	parse_linked_list(lines, song);
	free_list(lines);
}