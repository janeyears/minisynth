#ifndef SOUNDS_H
# define SOUNDS_H

# include "minisynth.h"

int		get_schedule(t_schedule *schedule, t_song *song);
int		paCallback(const void *inputBuffer, void *outputBuffer,
					unsigned long framesPerBuffer,
					const PaStreamCallbackTimeInfo* timeInfo,
					PaStreamCallbackFlags statusFlags,
					void *userData);

#endif