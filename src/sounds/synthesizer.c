#include "minisynth.h"

static void	envelope(double *sample, t_scheduled_track *track,
				double callback_time, t_scheduled_note *note)
{
	double time;
	double note_duration;
	double env_amp;

	time = callback_time - note->start_s;
	note_duration = note->end_s - note->start_s;
	env_amp = 1.0;

	if (time < track->attack)
		env_amp = time / track->attack;													// Attack
	else if (time < track->attack + track->decay)
		env_amp = 1.0 - (1.0 - track->sustain) * (time - track->attack) / track->decay;	// Decay
	else if (time < note_duration)
		env_amp = track->sustain;														// Sustain
	else
	{
		double release_time = time - note_duration;
		if (release_time < track->release)
			env_amp = track->sustain * (1.0 - release_time / track->release);			// Release
		else
			env_amp = 0.0;
	}

	// Add to output, scaled by volume and envelope
	*sample *= env_amp * track->volume;
}

static void	instrument(double *sample, t_scheduled_track *track,
				double callback_time, t_scheduled_note *note)
{
	double phase = TWO_PI * note->freq_hz * (callback_time - note->start_s);
	switch (track->instrument)
	{
		case SINE:
			*sample += sin(phase);
			break;
		case SQUARE:
			*sample += (sin(phase) >= 0 ? 1.0 : -1.0);
			break;
		case SAW:
			*sample += 2.0*(phase/(TWO_PI) - floor(phase/(TWO_PI) + 0.5));
			break;
		case TRIANGLE:
			*sample += asin(sin(phase))*PI_TWO;
			break;
		 case KICK:										// low frequency sine, ignore note->freq_hz
			*sample += sin(TWO_PI * 60.0 * (callback_time - note->start_s));
			break;
		case SNARE:										// white noise, ignore note->freq_hz
			*sample += ((double)rand() / RAND_MAX) * 2.0 - 1.0;
			break;
	}
}

int	paCallback(const void *inputBuffer, void *outputBuffer,
					unsigned long framesPerBuffer,
					const PaStreamCallbackTimeInfo* timeInfo,
					PaStreamCallbackFlags statusFlags,
					void *userData)
{
	int16_t            *out;
	t_schedule        *sched;
	double            callback_time;
	static double    bar_accum[BARS];   // sum of samples per bar
	static int        bar_count[BARS];
	int                levels[BARS];

	(void) inputBuffer;                                        //microphone
	(void) timeInfo;
	(void) statusFlags;

	sched = (t_schedule*)userData;
	out = (int16_t*)outputBuffer;
	callback_time = sched->current_time;

	for (unsigned long i = 0; i < framesPerBuffer; i++)		// Loop over buffer
	{
		double sample = 0.0;
		for (int t_idx = 0; t_idx < sched->track_count; t_idx++)	// Loop over all tracks
		{
			t_scheduled_track *track = &sched->tracks[t_idx];
			for (int n = 0; n < track->note_count; n++)
			{
				t_scheduled_note *note = &track->notes[n];
				if (callback_time >= note->start_s && callback_time < note->end_s)
				{
					if (!note->is_rest)
					{
						instrument(&sample, track, callback_time, note);
						envelope(&sample, track, callback_time, note);
						int bar_idx = (i * BARS) / framesPerBuffer;
						if (bar_idx >= BARS)
							bar_idx = BARS - 1;
						bar_accum[bar_idx] += fabs(sample); // accumulate absolute value (magnitude)
						bar_count[bar_idx]++;
					}
					break;			// only one note active at a time per track
				}
			}
		}
		// Scale down volume to prevent clipping
		sample *= 0.2;
		if (sample > 1.0) sample = 1.0;
		if (sample < -1.0) sample = -1.0;

		int16_t out_sample = (int16_t)(sample * 32767);
		*out++ = out_sample; // left
		*out++ = out_sample; // right
		callback_time += 1.0 / SAMPLE_RATE;
	}
	compute_levels(bar_accum, bar_count, levels);
	print_bar(levels);
	sched->current_time = callback_time;
	return paContinue; // keep playing
}
