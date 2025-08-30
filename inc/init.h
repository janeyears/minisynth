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

#define BARS 20
#define HEIGHT 10
#define MAX_FREQ 20000.0
#define DECAY 0.85
#define MAX_MAGNITUDE 32767.0

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[38;5;129m"
#define CYAN    "\033[36m"
#define ORANGE    "\033[38;5;208m"
#define LIGHT_RED     "\033[91m"
#define LIGHT_GREEN   "\033[92m"
#define LIGHT_YELLOW  "\033[93m"
#define LIGHT_BLUE    "\033[94m"
typedef enum e_inst
{
	SINE,
	SAW,
	SQUARE,
	TRIANGLE,
	KICK,
	SNARE
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
	int				octave;
	double				duration;
}	t_note;

typedef struct s_track
{
	t_inst				instrument;
	t_note				*notes;
	int					note_count;
	int					note_capacity;
	double				volume;		// 0.0 = silent, 1.0 = full volume
	double				attack;		// seconds
	double				decay;		// seconds
	double				sustain;	// 0.0–1.0
	double				release;	// seconds
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
	double				volume;		// 0.0 = silent, 1.0 = full volume
	double				attack;		// seconds
	double				decay;		// seconds
	double				sustain;	// 0.0–1.0
	double				release;	// seconds
} t_scheduled_track;

typedef struct s_schedule
{
	int					track_count;
	t_scheduled_track	*tracks;
	double				song_duration;
	double				current_time;
} t_schedule;

#endif