#ifndef MINISYNTH_H
# define MINISYNTH_H

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE 4096


typedef struct s_list {
	char *line;
	struct s_list *next;
} t_list;

typedef enum {
	SINE,
	SAW,
	SQUARE,
	TRIANGLE
} inst;

typedef struct s_note
{
	char pitch;
	char alteration;
	char octave;
	double duration;
}	t_note;

typedef struct s_track
{
	inst instrument;
	t_note *notes;
	int note_count;
	int note_capacity;
}	t_track;

typedef struct s_song
{
	int	tempo;
	int track_count;
	t_track *tracks;
}	t_song;

void parsing(char *file, t_song *song);

#endif