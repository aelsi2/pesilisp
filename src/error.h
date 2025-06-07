#pragma once

#include "location.h"
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
    location_t location;
    int exit_code;
} error_t;

error_t *error_exit(int code);
error_t *error_syntax(const location_t *loc, const char *message, ...);
error_t *error_usage(const location_t *loc, const char *message, ...);
error_t *error_runtime(const location_t *loc, const char *message, ...);

void error_free(error_t *error);

void error_print(error_t *error, FILE *file);
