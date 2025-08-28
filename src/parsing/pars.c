#include "minisynth.h"

// helper: trim leading/trailing spaces
char *trim(char *str) {
	char *end;

	// skip leading spaces
	while (isspace((unsigned char)*str)) str++;

	if (*str == 0)  // only spaces
		return str;

	// trim trailing spaces
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) end--;

	*(end + 1) = '\0';
	return str;
}

inst parse_instrument(const char *name) {
	if (strcmp(name, "sine") == 0) 
		return SINE;
	if (strcmp(name, "saw") == 0) 
		return SAW;
	if (strcmp(name, "square") == 0) 
		return SQUARE;
	if (strcmp(name, "triangle") == 0) 
		return TRIANGLE;
	else
		return NULL;
}

void check_extension(const char *file) 
{
	const char *dot = strrchr(file, '.');

	if (!dot || strcmp(dot, ".synth") != 0) {
		perror("Error: file must have .synth extension\n");
		exit(1);
	}
}

void add_note(track *track, note n) {
	if (track->note_count >= track->note_capacity) {
		track->note_capacity = (track->note_capacity == 0) ? 8 : track->note_capacity * 2;
		track->notes = realloc(track->notes, track->note_capacity * sizeof(note));
		if (!track->notes) {
			perror("realloc");
			exit(1);
		}
	}
	track->notes[track->note_count++] = n;
}

// parse a single note token
note parse_note_token(const char *token, int *prev_octave, double *prev_duration) {
	note n;
	n.pitch = token[0];
	n.alteration = '\0';
	n.octave = *prev_octave;
	n.duration = *prev_duration;

	int i = 1;

	// alteration
	if (token[i] == '#' || token[i] == 'b') {
		n.alteration = token[i];
		i++;
	}

	// octave
	if (isdigit((unsigned char)token[i])) {
		n.octave = token[i] - '0';
		*prev_octave = n.octave;
		i++;
	}

	// duration
	if (token[i] == '/') {
		n.duration = atof(token + i + 1);
		*prev_duration = n.duration;
	}
	return n;
	}

void read_file(char *file, song *song)
{
	int		fd;
	char	buffer[MAX_LINE];
	int		line_number = 0;
	int		tempo_found = 0;
	int		tracks_found = 0;

	fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		perror("Can't open the file\n");
		exit(1);
	}
	while (fgets(buffer, sizeof(buffer), fd)) 
	{
		char *line = trim(buffer);
		if (*line == '\0' || *line == '#')
			continue;
		if (!tempo_found && strncmp(line, "tempo", 5) == 0) 
		{
			tempo_found = 1;
			song->tempo = atoi(line + 5);
			continue;
		}
		if (!tracks_found && strncmp(line, "tracks", 6) == 0) 
		{
			tracks_found = 1;

			// count tracks
			char *instr_list = line + 6; // skip "tracks"
			while (*instr_list && (*instr_list == ' ' || *instr_list == '\t'))
				instr_list++;  // skip spaces

			// Count commas to determine track count
			int track_count = 1;
			for (char *p = instr_list; *p; p++)
			{
					if (*p == ',') 
					track_count++;
			}
			song->track_count = track_count;
			song->tracks = calloc(track_count, sizeof(track)); // allocate Track array

			// Initialize each track instrument
			int i = 0;
			char *token = strtok(instr_list, ",");
			while (token) {
				song->tracks[i].instrument = parse_instrument(trim(token)); // map to enum
				song->tracks[i].notes = NULL;
				song->tracks[i].note_count = 0;
				song->tracks[i].note_capacity = 0;
				i++;
				token = strtok(NULL, ",");
			}

			continue;
		}
		 // track notes
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

		// defaults reset per line
		int prev_octave = 4;
		double prev_duration = 1.0;

		char *note_token = strtok(notes_str, " \t|"); // also ignore '|'
		while (note_token) {
			note n = parse_note_token(note_token, &prev_octave, &prev_duration);
			add_note(&song->tracks[track_num], n);
			note_token = strtok(NULL, " \t|");
		}
	}
}

void parsing(char *file, song *song)
{
	check_extension(file);
	read_file(file, song);
}