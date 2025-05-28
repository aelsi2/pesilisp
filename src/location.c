#include "location.h"
#include <stdlib.h>
#include <string.h>

void location_init(location_t *loc, const char *file_name, int line,
                   int column) {
    if (file_name) {
        loc->file_name = malloc(strlen(file_name) + 1);
        strcpy(loc->file_name, file_name);
    } else {
        loc->file_name = NULL;
    }
    loc->line = line;
    loc->column = column;
}

void location_free(location_t *loc) {
    free(loc->file_name);
}

void location_copy(location_t *dest, const location_t *src) {
    if (src) {
        location_init(dest, src->file_name, src->line, src->column);
    } else {
        location_init(dest, NULL, 1, 1);
    }
}

void location_copy_temp(location_t *dest, const location_t *src) {
    if (src) {
        dest->file_name = src->file_name;
        dest->line = src->line;
        dest->column = src->column;
    } else {
        location_init(dest, NULL, 1, 1);
    }
}
