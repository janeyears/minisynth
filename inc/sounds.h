#ifndef SOUNDS_H
# define SOUNDS_H

# include "minisynth.h"
#include <stdint.h>
#include <math.h> 

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

// "Расписание" одной ноты после конверсии
typedef struct s_scheduled_note
{
	int is_rest;       // 1 = пауза, 0 = нота
	double start_s;    // время начала в секундах
	double end_s;      // время конца в секундах
	double freq_hz;    // частота ноты (0 для паузы)
} t_scheduled_note;

// Трек с нотами в секундах
typedef struct s_scheduled_track
{
	inst instrument;       // SINE, SAW, SQUARE, TRIANGLE
	t_scheduled_note *notes;  // массив запланированных нот
	int note_count;        // текущее количество нот
} t_scheduled_track;

// Песня с рассчитанными треками
typedef struct s_schedule
{
	int track_count;        // количество треков
	t_scheduled_track *tracks; // массив треков
	double song_duration;         // общая длительность песни
} t_schedule;

int 		get_schedule(t_schedule *schedule, t_song *song);
static int	paCallback(const void *inputBuffer, void *outputBuffer,
					unsigned long framesPerBuffer,
					const PaStreamCallbackTimeInfo* timeInfo,
					PaStreamCallbackFlags statusFlags,
					void *userData);

#endif