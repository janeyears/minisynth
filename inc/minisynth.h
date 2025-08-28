#ifndef MINISYNTH_H
# define MINISYNTH_H

#include <stdio.h>
#include <fcntl.h>

#define MAX_LINE 4096

typedef enum {
	SINE,
	SAW,
	SQUARE,
	TRIANGLE
} inst;

typedef struct
{
	char pitch;
	char alteration;
	char octave;
	char duration;
}	note;

typedef struct
{
	inst instrument;
	note *notes;
	int note_count;
	int note_capacity;
}	track;

typedef struct
{
	int	tempo;
	int track_count;
	track *tracks;
}	song;

#endif