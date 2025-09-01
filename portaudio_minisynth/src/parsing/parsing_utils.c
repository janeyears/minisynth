#include "minisynth.h"

void free_list(t_list *head)
{
	t_list *tmp;
	while (head) {
		tmp = head->next;
		if (head->line)
			free(head->line);
		free(head);
		head = tmp;
	}
}
int is_note_letter(char c) 
{
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

void free_notes(t_song *song) 
{
	for (int i = 0; i < song->track_count; i++)
		free(song->tracks[i].notes);
}

void free_song(t_song *song)
{
	free_notes(song);
	free(song->tracks);
}

void free_schedule(t_schedule *schedule) 
{
	for (int j = 0; j < schedule->track_count; j++)
	{
		for (int i = 0; i < schedule->tracks[j].note_count; i++)
			free(schedule->tracks[i].notes);
	}
	free(schedule->tracks);
}