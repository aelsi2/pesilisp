#pragma once

typedef struct {
    char *file_name;
    int line;
    int column;
} location_t;

static inline void location_next_column(location_t *loc) {
    loc->column++;
}
static inline void location_next_line(location_t *loc) {
    loc->line++;
    loc->column = 0;
}
void location_init(location_t *loc, const char *file_name, int line,
                   int column);
void location_free(location_t *loc);
void location_copy(location_t *dest, const location_t *src);
void location_copy_temp(location_t *dest, const location_t *src);
