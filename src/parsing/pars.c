#include "minisynth.h"

int is_note_letter(char c) {
    return (c >= 'a' && c <= 'g') || c == 'k' || c == 's';
}

char *trim(char *str)
{
	char *end;
	while (isspace((unsigned char)*str)) 
		str++;
	if (*str == 0) 
		return str;
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) 
		end--;
	*(end + 1) = '\0';
	return str;
}

t_inst parse_instrument(const char *name) {
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

t_note parse_note_token(const char *token, int *prev_octave, double *prev_duration) 
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

void add_note(t_track *track, t_note n) 
{
	if (track->note_count >= track->note_capacity) 
	{
		track->note_capacity = (track->note_capacity == 0) ? 8 : track->note_capacity * 2;
		track->notes = realloc(track->notes, track->note_capacity * sizeof(t_note));
		if (!track->notes) { perror("realloc"); exit(1); }
	}
	track->notes[track->note_count++] = n;
}

// ---------------- Phase 1: Read file into linked list ----------------

t_list *read_file_to_list(const char *filename) 
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
		char *line = trim(buffer);
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

// ---------------- Phase 2: Parse linked list into structs ----------------

void parse_linked_list(t_list *head, t_song *song) 
{
	t_list *curr = head;
	int tempo_found = 0;
	int tracks_found = 0;

	while (curr) 
	{
		char *line = curr->line;

		if (!tempo_found && strncmp(line, "tempo", 5) == 0) {
			tempo_found = 1;
			song->tempo = atoi(line + 5);
		}
		else if (strncmp(line, "volumes", 7) == 0) 
		{
			char *vol_str = trim(line + 7);  // skip "volumes"
			int i = 0;
			char *token = strtok(vol_str, ",");
			while (token && i < song->track_count) 
			{
				double vol = atof(trim(token));
				song->tracks[i].volume = vol; // store 0.0 - 1.0
				i++;
				token = strtok(NULL, ",");
			}
			if (i != song->track_count)
			{
				fprintf(stderr, "Volume count does not match track count\n");
				exit(1);
			}
		}	
		else if (!tracks_found && strncmp(line, "tracks", 6) == 0)
		{
			tracks_found = 1;
			char *instr_list = trim(line + 6);
			int track_count = 1;
			for (char *p = instr_list; *p; p++)
			{
				if (*p == ',') 
					track_count++;
			}
			song->track_count = track_count;
			song->tracks = calloc(track_count, sizeof(t_track));

			int i = 0;
			char *token = strtok(instr_list, ",");
			while (token) 
			{
				song->tracks[i].instrument = parse_instrument(trim(token));
				song->tracks[i].notes = NULL;
				song->tracks[i].note_count = 0;
				song->tracks[i].note_capacity = 0;
				song->tracks[i].volume = 100;  // default 100%
				song->tracks[i].attack  = 0.01; // optional default ADSR
				song->tracks[i].decay   = 0.1;
				song->tracks[i].sustain = 0.8;
				song->tracks[i].release = 0.3;
				i++;
				token = strtok(NULL, ",");
			}
		}
		else if (strncmp(line, "adsr", 4) == 0) 
		{
			int track_num;
			double a, d, s, r;
			if (sscanf(line, "adsr track %d %lf %lf %lf %lf", &track_num, &a, &d, &s, &r) != 5)
			{
				fprintf(stderr, "Invalid ADSR line: %s\n", line);
				exit(1);
			}
			track_num -= 1; // adjust to 0-based index
			if (track_num < 0 || track_num >= song->track_count)
			{
				fprintf(stderr, "ADSR track number out of range: %d\n", track_num + 1);
				exit(1);
			}
			song->tracks[track_num].attack  = a;
			song->tracks[track_num].decay   = d;
			song->tracks[track_num].sustain = s;
			song->tracks[track_num].release = r;

		}	
		else 
		{
		// Only parse track lines here
			if (isdigit((unsigned char)line[0])) 
			{
				char *colon = strchr(line, ':');
				if (!colon) { 
					fprintf(stderr, "Invalid track line: %s\n", line); 
					exit(1); 
				}
				*colon = '\0';
				int track_num = atoi(line) - 1;
				if (track_num < 0 || track_num >= song->track_count) {
					fprintf(stderr, "Track number out of range: %d\n", track_num + 1);
					exit(1);
				}
				char *notes_str = trim(colon + 1);
				int prev_octave = 4;
				double prev_duration = 1.0;
				char prev_pitch = 'c'; // default first note of line

				char *note_token = strtok(notes_str, " \t|");
				while (note_token) {
					t_note n = parse_note_token(note_token, &prev_octave, &prev_duration);
					// handle pitch inheritance
					if (is_note_letter(n.pitch) || n.pitch == 'r')
						prev_pitch = n.pitch;
					else 
						n.pitch = prev_pitch;
					add_note(&song->tracks[track_num], n);
					note_token = strtok(NULL, " \t|");
				}
			}
		}
		curr = curr->next;
	}
}

void free_line_list(t_list *head)
{
	while (head) {
		t_list *tmp = head;
		head = head->next;
		free(tmp->line);
		free(tmp);
	}
}

// void print_note(const t_note *n) {
//     if (n->pitch == 'r') {
//         printf("Rest (dur=%.4f)\n", n->duration);
//     } else {
//         printf("Pitch=%c%s Oct=%d Dur=%.4f\n",
//             n->pitch,
//             (n->alteration == '#' ? "#" : (n->alteration == 'b' ? "b" : "")),
//             n->octave,
//             n->duration);
//     }
// }

// void dump_track(const t_track *t, int index) {
//     const char *inst_names[] = { "SINE", "SAW", "SQUARE", "TRIANGLE" };
//     printf("Track %d (Instrument=%s, Notes=%d):\n", 
//            index + 1, inst_names[t->instrument], t->note_count);

//     for (int i = 0; i < t->note_count; i++) {
//         printf("  [%d] ", i);
//         print_note(&t->notes[i]);

//         if (i >= 9) { // only print first 10 notes
//             printf("  ... (%d more)\n", t->note_count - 10);
//             break;
//         }
//     }
// }

void parsing(char *file, t_song *song) {
	t_list *lines = read_file_to_list(file);
	parse_linked_list(lines, song);
	free_line_list(lines);

	// printf("tempo: %d\n", song->tempo);
	// printf("track_count: %d\n", song->track_count);
	// for (int i = 0; i < song->track_count; i++) {
    //     dump_track(&song->tracks[i], i);
    // }
}