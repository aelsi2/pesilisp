#pragma once

#include <stdio.h>

#define COLOR_RED 31
#define COLOR_GREEN 32 
#define COLOR_BLUE 34

void begin_color(FILE *file, int color);
void end_color(FILE *file);
