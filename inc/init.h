#ifndef INIT_H
# define INIT_H

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100.0
#endif

#ifndef FRAMES_PER_BUFFER
#define FRAMES_PER_BUFFER 256
#endif

#ifndef CHANNELS
#define CHANNELS 2
#endif

#ifndef BUFFER
#define BUFFER 512
#endif

#define TWO_PI 6.283185307179586
#define PI_TWO 0.6366197723675814

#define MAX_LINE 4096

typedef enum e_inst
{
	SINE,
	SAW,
	SQUARE,
	TRIANGLE
} t_inst;

// Parsing

typedef struct s_list {
	char				*line;
	struct s_list		*next;
} t_list;

typedef struct s_note
{
	char				pitch;
	char				alteration;
	char				octave;
	double				duration;
}	t_note;

typedef struct s_track
{
	t_inst				instrument;
	t_note				*notes;
	int					note_count;
	int					note_capacity;
}	t_track;

typedef struct s_song
{
	int					tempo;
	int					track_count;
	t_track				*tracks;
}	t_song;

// Schedule

typedef struct s_scheduled_note
{
	int					is_rest;	// 1 = pause, 0 = note
	double				start_s;	// start of note playing
	double				end_s;		// end of note playing
	double				freq_hz;
} t_scheduled_note;

typedef struct s_scheduled_track
{
	t_inst				instrument;
	t_scheduled_note	*notes;
	int					note_count;
} t_scheduled_track;

typedef struct s_schedule
{
	int					track_count;
	t_scheduled_track	*tracks;
	double				song_duration;
} t_schedule;

#endif