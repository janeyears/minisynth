#include "minisynth.h"

int main (int ac, char **av)
{
	t_song song;

	song.tempo = 0;
	song.track_count = 0;
	song.tracks = NULL;

	if (ac == 2)
		parsing(av[1], &song);						// Parse file
	else
		return (perror("Usage: ./minisynth file\n"), 1);

	t_schedule schedule;							// Fill schedule for tracks by notes
	get_schedule(&schedule, &song);
	write_schedule_to_wav("output.wav", &schedule);	// Write music to .wav, because I want

	PaStream *stream;
	PaError err;
	err = Pa_Initialize();							// Initialization of PortAudio
	if (err != paNoError)
		return (perror("PortAudio error: %s"), perror(Pa_GetErrorText(err)), 1);

	printf("PortAudio initialized!\n");

	err = Pa_OpenDefaultStream(&stream,				// Open a stream with parameters:
							0,						// no input channels (mic)
							2,						// 2 output channels (stereo)
							paInt16,				// 16-bit int samples
							SAMPLE_RATE,			// 44100 Hz sample rate
							FRAMES_PER_BUFFER,
							paCallback,				// our callback
							&schedule);				// user data, my schedule

	if (err != paNoError)
		return (perror("Open error\n"), 1);

	err = Pa_StartStream(stream);					// Start stream (playing)
	if (err != paNoError)
		return (printf("Start error\n"), 1);

	Pa_Sleep((int)(schedule.song_duration * 1000));	// Wait for whole song to play

	Pa_StopStream(stream);							// Stop and close stream
	Pa_CloseStream(stream);
	Pa_Terminate();									//Terminate when done
	printf("Hope you enjoed!\n");
}