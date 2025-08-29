#include "minisynth.h"

void write_wav_header(FILE *f, int num_samples, int num_channels, int sample_rate)	// Write WAV header
{
	int32_t chunk_size = 36 + num_samples * num_channels * 2;
	int16_t audio_format = 1; // PCM
	int16_t bits_per_sample = 16;
	int32_t byte_rate = sample_rate * num_channels * bits_per_sample / 8;
	int16_t block_align = num_channels * bits_per_sample / 8;

	fwrite("RIFF", 1, 4, f);
	fwrite(&chunk_size, 4, 1, f);
	fwrite("WAVE", 1, 4, f);

	fwrite("fmt ", 1, 4, f);
	int32_t subchunk1_size = 16;
	fwrite(&subchunk1_size, 4, 1, f);
	fwrite(&audio_format, 2, 1, f);
	fwrite(&num_channels, 2, 1, f);
	fwrite(&sample_rate, 4, 1, f);
	fwrite(&byte_rate, 4, 1, f);
	fwrite(&block_align, 2, 1, f);
	fwrite(&bits_per_sample, 2, 1, f);

	fwrite("data", 1, 4, f);
	int32_t subchunk2_size = num_samples * num_channels * 2;
	fwrite(&subchunk2_size, 4, 1, f);
}

// Convert t_schedule to WAV
void write_schedule_to_wav(const char *filename, t_schedule *schedule)
{
	FILE *f = fopen(filename, "wb");
	if (!f) return;

	int num_channels = 2;
	int num_samples = (int)(schedule->song_duration * SAMPLE_RATE);
	write_wav_header(f, num_samples, num_channels, SAMPLE_RATE); 

	for (int i = 0; i < num_samples; i++) {
		double t = (double)i / SAMPLE_RATE;
		double sample = 0.0;

		// Sum all tracks
		for (int tr = 0; tr < schedule->track_count; tr++) {
			t_scheduled_track *track = &schedule->tracks[tr];

			for (int n = 0; n < track->note_count; n++) {
				t_scheduled_note *note = &track->notes[n];

				if (!note->is_rest && t >= note->start_s && t < note->end_s) {
					double phase = TWO_PI * note->freq_hz * (t - note->start_s);
					switch (track->instrument) {
						case SINE: sample += sin(phase); break;
						case SQUARE: sample += (sin(phase) >= 0 ? 1.0 : -1.0); break;
						case SAW: sample += 2.0 * (t * note->freq_hz - floor(t * note->freq_hz + 0.5)); break;
						case TRIANGLE: sample += asin(sin(phase)) * PI_TWO; break;
					}
					sample *= track->volume;
				}
			}
		}

		// Scale down volume
		sample *= 0.2;
		if (sample > 1.0) sample = 1.0;
		if (sample < -1.0) sample = -1.0;

		int16_t s = (int16_t)(sample * 32767);
		fwrite(&s, 2, 1, f); // left
		fwrite(&s, 2, 1, f); // right
	}

	fclose(f);
}