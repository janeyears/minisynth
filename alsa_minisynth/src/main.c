#include "minisynth.h"

t_schedule schedule;

int main(int ac, char **av)
{
	t_song song;
	song.tempo = 0;
	song.track_count = 0;
	song.tracks = NULL;

	if (ac == 2)
		parsing(av[1], &song);
	else
		return (perror("Usage: ./minisynth_file <song.synth>\n"), 1);

	get_schedule(&schedule, &song);

	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;

	if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		perror("ALSA: cannot open device");
		free_schedule(&schedule);
		free_song(&song);
		return 1;
	}
	printf("ALSA initialized!\n");

	snd_pcm_hw_params_malloc(&params);
	snd_pcm_hw_params_any(pcm_handle, params);
	snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(pcm_handle, params, 2);

	unsigned int rate = SAMPLE_RATE;
	snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);

	snd_pcm_uframes_t period = FRAMES_PER_BUFFER;
	snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &period, 0);
	snd_pcm_uframes_t buffer_size = period * 4;
	snd_pcm_hw_params_set_buffer_size_near(pcm_handle, params, &buffer_size);

	int err = snd_pcm_hw_params(pcm_handle, params);
	if (err < 0) { fprintf(stderr, "hw_params: %s\n", snd_strerror(err)); return 1; }
	snd_pcm_hw_params_free(params);

	snd_pcm_prepare(pcm_handle);

	int16_t buffer[FRAMES_PER_BUFFER * 2];

	while (schedule.current_time < schedule.song_duration) {
		snd_pcm_sframes_t avail = snd_pcm_avail_update(pcm_handle);
		if (avail < 0)
		{
			if (avail == -EPIPE)
			{
				snd_pcm_prepare(pcm_handle);
				continue;
			}
			else
			{
				fprintf(stderr, "avail_update error: %s\n", snd_strerror(avail));
				break;
			}
		}

		if ((unsigned long)avail >= FRAMES_PER_BUFFER)
		{
			paCallback(buffer, FRAMES_PER_BUFFER, &schedule);

			err = snd_pcm_writei(pcm_handle, buffer, FRAMES_PER_BUFFER);
			if (err < 0)
			{
				if (err == -EPIPE)
					snd_pcm_prepare(pcm_handle);
				else
				{
					fprintf(stderr, "write error: %s\n", snd_strerror(err));
					break;
				}
			}
			print_bar(buffer);
		}
		else
			snd_pcm_wait(pcm_handle, 100);
	}

	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);

	printf("\033[1;5;31mHope you enjoyed!%s\n", RESET);
	free_schedule(&schedule);
	free_song(&song);
	return 0;
}
