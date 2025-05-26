#pragma once

#include <stdio.h>

typedef enum {
    ERROR_SYNTAX,
    ERROR_USAGE,
    ERROR_RUNTIME,
    ERROR_EXIT,
} error_type_t;

typedef struct {
    error_type_t type;
    char *message;
    char *file;
    int line;
    int col;
    int exit_code;
} error_t;

error_t *error_exit(int code);

error_t *error_interactive();

void error_free(error_t *error);

void error_print(error_t *error, FILE *file);

