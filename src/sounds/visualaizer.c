#include "minisynth.h"

static const char* row_to_color(int row)
{
    static const char* colors[HEIGHT] = {
        MAGENTA,          // bottom
        BLUE,
        LIGHT_BLUE,
        CYAN,
        GREEN,
        LIGHT_GREEN,
        LIGHT_YELLOW,
        ORANGE,
        LIGHT_RED,
        RED                // top
    };

    if (row < 1) row = 1;
    if (row > HEIGHT) row = HEIGHT;
    return colors[row - 1];
}

void print_bar(int levels[BARS])
{
    printf("\033[H\033[J"); // clear screen

    for (int row = 1; row <= HEIGHT; row++) {  // bottom row first
        for (int i = 0; i < BARS; i++) {
            if (levels[i] >= row) {           // draw if bar reaches this row
                printf("%s███%s ", row_to_color(row), RESET);
            } else {
                printf("    ");                // empty space
            }
        }
        printf("\n");
    }
}
void compute_levels(double bar_accum[BARS], int bar_count[BARS], int levels[BARS])
{
    for (int i = 0; i < BARS; i++) {
        if (bar_count[i] > 0) {
            double avg = bar_accum[i] / bar_count[i];   // average magnitude
            int h = (int)(avg * HEIGHT / 1.0);          // scale to HEIGHT
            if (h > HEIGHT) h = HEIGHT;
            levels[i] = h;
        } else {
            levels[i] = 0;
        }
        bar_accum[i] = 0;  // reset for next callback
        bar_count[i] = 0;
    }
}