#ifndef MINISYNTH_H
# define MINISYNTH_H

#include "init.h"
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h> 
#include "sounds.h"

void parsing(char *file, t_song *song);
void free_list(t_list *head);
int is_note_letter(char c);
char *trim(char *str);
void free_notes(t_song *song);

#endif
