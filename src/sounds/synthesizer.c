#include "minisynth.h"

int paCallback(const void *inputBuffer, void *outputBuffer,
					unsigned long framesPerBuffer,
					const PaStreamCallbackTimeInfo* timeInfo,
					PaStreamCallbackFlags statusFlags,
					void *userData)
{
	int16_t *out;
	t_schedule *sched;
	double callback_time; 

	(void) inputBuffer;										//microphone
		// Pointer to PaStreamCallbackTimeInfo struct:
		//   - inputBufferAdcTime: timestamp of the first sample of the input buffer (ignored here)
		//   - currentTime: estimated current stream time in seconds (ignored here)
		//   - outputBufferDacTime: timestamp of the first sample of the output buffer (ignored here)
	(void) timeInfo;
		// Bitmask indicating stream flags:
		//   - paInputUnderflow   : input buffer underflow occurred
		//   - paInputOverflow    : input buffer overflow occurred
		//   - paOutputUnderflow  : output buffer underflow occurred
		//   - paOutputOverflow   : output buffer overflow occurred
		//   - paPrimingOutput    : first callback after stream start
	(void) statusFlags;

	callback_time = 0.0;
	sched = (t_schedule*)userData;
	out = (int16_t*)outputBuffer;

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
					if (!note->is_rest) {
						double phase = TWO_PI * note->freq_hz * (callback_time - note->start_s);
						switch (track->instrument)
						{
							case SINE:
								sample += sin(phase);
								break;
							case SQUARE:
								sample += (sin(phase) >= 0 ? 1.0 : -1.0);
								break;
							case SAW:
								sample += 2.0*(phase/(TWO_PI) - floor(phase/(TWO_PI) + 0.5));
								break;
							case TRIANGLE:
								sample += asin(sin(phase))*PI_TWO;
								break;
						}
					}
					break;							// only one note active at a time per track
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
	return paContinue; // keep playing
}