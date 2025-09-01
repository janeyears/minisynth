#include "minisynth.h"

static const char* row_to_color(int row)
{
	static const char* colors[HEIGHT] = {
		MAGENTA,          // top
		BLUE,
		LIGHT_BLUE,
		CYAN,
		GREEN,
		LIGHT_GREEN,
		LIGHT_YELLOW,
		ORANGE,
		LIGHT_RED,
		RED                // bottom
	};

	if (row < 1) row = 1;
	if (row > HEIGHT) row = HEIGHT;
	return colors[row - 1];
}

void print_bar(int16_t *buffer)
{
	int levels[BARS] = {0};
	unsigned long samples_per_bar = FRAMES_PER_BUFFER / BARS;

	for (int i = 0; i < BARS; i++) {
		int16_t max_amp = 0;
		for (unsigned long j = 0; j < samples_per_bar; j++) {
			unsigned long idx = i * samples_per_bar + j;
			if (idx >= FRAMES_PER_BUFFER) break;
			int16_t abs_sample = buffer[idx] < 0 ? -buffer[idx] : buffer[idx];
			if (abs_sample > max_amp) max_amp = abs_sample;
		}
		levels[i] = max_amp * HEIGHT / 32767;
		if (levels[i] > HEIGHT) levels[i] = HEIGHT;
	}

	printf("\033[H\033[J");						// Clear
	for (int row = 1; row <= HEIGHT; row++)
	{
		for (int i = 0; i < BARS; i++)
		{
			if (levels[i] >= row)
				printf("%s███%s ", row_to_color(row), RESET);
			else
				printf("    ");
		}
		printf("\n");
	}
}
