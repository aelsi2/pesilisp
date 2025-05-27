#include "error.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static error_t *error_new(error_type_t type, const location_t *location,
                          int exit_code, const char *message) {
    error_t *error = malloc(sizeof(error_t));
    error->type = type;
    error->exit_code = exit_code;
    location_copy(&error->location, location);
    if (message) {
        error->message = malloc(strlen(message) + 1);
        strcpy(error->message, message);
    } else {
        error->message = NULL;
    }
    return error;
}

static error_t *error_new_fmt(error_type_t type, const location_t *location,
                              int exit_code, const char *message_format,
                              va_list args) {
    error_t *error = malloc(sizeof(error_t));
    error->type = type;
    error->exit_code = exit_code;
    location_copy(&error->location, location);
    if (message_format) {
        va_list nargs;
        va_copy(nargs, args);
        size_t msg_len = vsnprintf(NULL, 0, message_format, nargs);
        va_end(nargs);
        error->message = malloc(msg_len + 1);
        vsprintf(error->message, message_format, args);
    } else {
        error->message = NULL;
    }
    return error;
}

error_t *error_exit(int code) {
    return error_new(ERROR_EXIT, NULL, code, NULL);
}

error_t *error_syntax(const location_t *loc, const char *message, ...) {
    va_list args;
    va_start(args, message);
    error_t *error = error_new_fmt(ERROR_SYNTAX, loc, 1, message, args);
    va_end(args);
    return error;
}

error_t *error_usage(const location_t *loc, const char *message, ...) {
    va_list args;
    va_start(args, message);
    error_t *error = error_new_fmt(ERROR_USAGE, loc, 1, message, args);
    va_end(args);
    return error;
}

error_t *error_runtime(const location_t *loc, const char *message, ...) {
    va_list args;
    va_start(args, message);
    error_t *error = error_new_fmt(ERROR_RUNTIME, loc, 1, message, args);
    va_end(args);
    return error;
}

void error_free(error_t *error) {
    if (!error) {
        return;
    }
    free(error->message);
    free(error->location.file_name);
    free(error);
}

void error_print(error_t *error, FILE *file) {
    if (!error) {
        fprintf(file, "Unknown error.\n");
        return;
    }
    switch (error->type) {
    case ERROR_SYNTAX:
        fprintf(file, "Syntax error");
        break;
    case ERROR_USAGE:
        fprintf(file, "Usage error");
        break;
    case ERROR_RUNTIME:
        fprintf(file, "Runtime error");
        break;
    case ERROR_EXIT:
        fprintf(file, "Exit [code %d]", error->exit_code);
        break;
    }
    if (error->location.file_name) {
        char *file_name = error->location.file_name;
        int line = error->location.line;
        int column = error->location.column;
        fprintf(file, " at file %s, line %d, column %d", file_name, line,
                column);
    }
    if (error->message) {
        fprintf(file, ": \n%s\n", error->message);
    } else {
        fprintf(file, ".\n");
    }
}
