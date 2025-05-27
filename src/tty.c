#include "tty.h"
#include <stdbool.h>
#ifdef linux
#include <unistd.h>
#endif

#define ANSI_COLOR "\x1b[%dm"

static bool is_tty(FILE *file) {
#ifdef linux
    return isatty(fileno(file));
#else
    return false;
#endif
}

void begin_color(FILE *file, int color) {
    if (is_tty(file)) {
        fprintf(file, ANSI_COLOR, color);
    }
}


void end_color(FILE *file) {
    if (is_tty(file)) {
        fprintf(file, ANSI_COLOR, 0);
    }
}
