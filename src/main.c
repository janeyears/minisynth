#include "minisynth.h"

int main (int ac, char **av)
{
	t_song song;

	song.tempo = 0;
	song.track_count = 0;
	song.tracks = NULL;

	if (ac == 2)
		parsing(av[1], &song);
	else
	{
		perror("Usage: ./minisynth file\n");
		return 1;
	}
}