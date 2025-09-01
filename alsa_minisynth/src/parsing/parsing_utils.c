#include "minisynth.h"

void free_list(t_list *head)
{
	while (head) {
		t_list *tmp = head;
		head = head->next;
		free(tmp->line);
		free(tmp);
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