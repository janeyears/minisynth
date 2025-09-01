#ifndef SOUNDS_H
# define SOUNDS_H

# include "minisynth.h"

int		get_schedule(t_schedule *schedule, t_song *song);
void	print_bar(int16_t *buffer);
void	paCallback(int16_t *outputBuffer, unsigned long framesPerBuffer, t_schedule *sched);

#endif
