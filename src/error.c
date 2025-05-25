#include "error.h"
#include <stdlib.h>

error_t *error_exit(int code) {
    error_t *error = malloc(sizeof(error_t));
    error->type = ERROR_EXIT;
    error->message = NULL;
    error->file = NULL;
    error->exit_code = code;
    return error;
}

void error_free(error_t *error) {
    if (!error) {
        return;
    }
    free(error->message);
    free(error->file);
    free(error);
}

void error_print(error_t *error, FILE *file) {
    if (!error) {
        fprintf(file, "Unknown error.\n");
        return;
    }
    if (error->file) {
        fprintf(file, "%s:%d:%d: ", error->file, error->line, error->col);
    }
    switch (error->type) {
    case ERROR_SYNTAX:
        fprintf(file, "Syntax error:\n");
        break;
    case ERROR_USAGE:
        fprintf(file, "Usage error:\n");
        break;
    case ERROR_RUNTIME:
        fprintf(file, "Runtime error:\n");
        break;
    case ERROR_EXIT:
        fprintf(file, "Exit (%d).\n", error->exit_code);
        break;
    }
    if (error->message) {
        fprintf(file, "%s", error->message);
    }
}
