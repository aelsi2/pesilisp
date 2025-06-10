#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#define TTY_UNIX
#elif defined(_WIN32)
#define TTY_WINDOWS
#endif

#include "tty.h"
#include <stdbool.h>

#ifdef TTY_UNIX
#include <unistd.h>
#elifdef TTY_WINDOWS
#include <windows.h>
#endif

#define ANSI_COLOR "\x1b[%dm"

void begin_color(FILE *file, int color) {
#ifdef TTY_UNIX
    if (isatty(fileno(file))) {
        fprintf(file, ANSI_COLOR, color);
    }
#elifdef TTY_WINDOWS
    WORD attributes;
    switch (color) {
        case COLOR_RED:
            attributes = FOREGROUND_RED;
            break;
        case COLOR_GREEN:
            attributes = FOREGROUND_GREEN;
            break;
        case COLOR_BLUE:
            attributes = FOREGROUND_BLUE;
            break;
    }
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(handle, &mode)) {
        SetConsoleTextAttribute(handle, attributes);
    }
#endif
}


void end_color(FILE *file) {
#ifdef TTY_UNIX
    if (isatty(fileno(file))) {
        fprintf(file, ANSI_COLOR, 0);
    }
#elifdef TTY_WINDOWS
    WORD attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(handle, &mode)) {
        SetConsoleTextAttribute(handle, attributes);
    }
#endif
}
