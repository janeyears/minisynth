#include "minisynth.h"

static void envelope(double *sample, t_scheduled_track *track,
					double callback_time, t_scheduled_note *note)
{
	double time = callback_time - note->start_s;
	double note_duration = note->end_s - note->start_s;
	double env_amp = 0.0;

	if (time < 0)
		env_amp = 0.0;
	else if (time < track->attack)
	{
		double t = time / track->attack;								// Smooth attack
		env_amp = track->sustain * t * t;
	}
	else if (time < track->attack + track->decay) {
		
		double t = (time - track->attack) / track->decay;				// Decay
		env_amp = track->sustain + (1.0 - track->sustain) * (1.0 - t);
	}
	else if (time < note_duration)
		env_amp = track->sustain;										// Sustain
	else
	{
		double release_time = time - note_duration;						// Smooth Release
		if (release_time < track->release) {
			double t = release_time / track->release;
			env_amp = track->sustain * (1.0 - t*t);
		} else {
			env_amp = 0.0;
		}
	}

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
			*sample += (((double)rand() / RAND_MAX) * 2.0 - 1.0) * 0.3;
			break;
	}
}

void paCallback(int16_t *outputBuffer, unsigned long framesPerBuffer, t_schedule *sched)
{
	int16_t *out = outputBuffer;
	double callback_time = sched->current_time;

	for (unsigned long i = 0; i < framesPerBuffer; i++) {
		double sample = 0.0;

		for (int t_idx = 0; t_idx < sched->track_count; t_idx++) {
			t_scheduled_track *track = &sched->tracks[t_idx];
			for (int n = 0; n < track->note_count; n++) {
				t_scheduled_note *note = &track->notes[n];
				if (callback_time >= note->start_s && callback_time < note->end_s && !note->is_rest) {
					instrument(&sample, track, callback_time, note);
					envelope(&sample, track, callback_time, note);
					break;
				}
			}
		}

		if (sample > 1.0) sample = 1.0;
		if (sample < -1.0) sample = -1.0;

		int16_t out_sample = (int16_t)(sample * 32767);
		*out++ = out_sample;
		*out++ = out_sample;

		callback_time += 1.0 / SAMPLE_RATE;
	}
	sched->current_time = callback_time;
}
