#include "minisynth.h"

int pitch_to_semitone(char pitch) {
	switch(pitch) {
		case 'c': return 0;
		case 'd': return 2;
		case 'e': return 4;
		case 'f': return 5;
		case 'g': return 7;
		case 'a': return 9;
		case 'b': return 11;
		default: return -1; // r
	}
}

/*pitch, alteration, octave to frequency*/

double note_to_freq(t_note n)
{
	int	semitone;
	int	octave;
	int	midi;
	double	freq;

	if(n.pitch == 'r')
		return 0.0; // пауза
	semitone = pitch_to_semitone(n.pitch);
	if(n.alteration == '#')
		semitone += 1;
	if(n.alteration == 'b')
		semitone -= 1;
	octave = n.octave;  // already an int now
	midi = (octave + 1) * 12 + semitone;
	freq = 440.0 * pow(2.0, (midi - 69) / 12.0);
	printf("%f, %d, %d, %d, %c, %c, %c\n", freq, octave, semitone, midi, n.alteration, n.pitch, n.octave);
	return (freq);
}

double max_duration(t_schedule *schedule)
{
	double	max_time;
	double	last;

	max_time = 0.0;
	for (int i = 0; i < schedule->track_count; i++) {
		if (schedule->tracks[i].note_count > 0) {
			last = schedule->tracks[i].notes[schedule->tracks[i].note_count - 1].end_s;
			if (last > max_time)
				max_time = last;
		}
	}
	return (max_time);
}

int get_schedule(t_schedule *schedule, t_song *song)
{
	double seconds_per_beat;
	double time;

	schedule->current_time = 0.0;
	seconds_per_beat = 60.0 / song->tempo;
	schedule->track_count = song->track_count; // from parsed data
	schedule->tracks = malloc(schedule->track_count * sizeof(t_scheduled_track));
	if (!schedule->tracks)
	{
		// perror("malloc failed");  ERROR MSG
		return (-1);
	}
	for (int i = 0; i < schedule->track_count; i++) {
		schedule->tracks[i].note_count = song->tracks[i].note_count;
		schedule->tracks[i].notes = malloc(
			sizeof(t_scheduled_note) * schedule->tracks[i].note_count);
		if (!schedule->tracks)
		{
			// perror("malloc failed");  ERROR MSG
			//free schedule->tracks[i].notes for all done i
			//free schedule->tracks
			return (-1);
		}
		schedule->tracks[i].instrument = song->tracks[i].instrument;

//NEED ADD TO PARSING
		schedule->tracks[i].volume = 1.0;//song->tracks[i].volume / 100.0;
		schedule->tracks[i].attack = 0.01;//song->tracks[i].attack;
		schedule->tracks[i].decay = 0.1;//song->tracks[i].decay;
		schedule->tracks[i].sustain = 0.8;//song->tracks[i].sustain;
		schedule->tracks[i].release = 0.3;//song->tracks[i].release;

		time = 0.0;
		for (int j = 0; j < song->tracks[i].note_count; j++) {
			schedule->tracks[i].notes[j].is_rest = 0;
			schedule->tracks[i].notes[j].freq_hz = note_to_freq(song->tracks[i].notes[j]);
			if (schedule->tracks[i].notes[j].freq_hz == 0.0)
				schedule->tracks[i].notes[j].is_rest = 1;
			schedule->tracks[i].notes[j].start_s = time;
			schedule->tracks[i].notes[j].end_s = time + song->tracks[i].notes[j].duration * seconds_per_beat;
			time = schedule->tracks[i].notes[j].end_s;
		}
	}
	schedule->song_duration = max_duration(schedule);
	return (1);
}
