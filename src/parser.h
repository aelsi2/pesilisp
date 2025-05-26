#pragma once

#include "data/object.h"
#include <stdbool.h>

typedef struct parser_s parser_t;

parser_t *parser_new(FILE *file, const char *file_name);

void parser_free(parser_t *parser);

bool parser_read_object(parser_t *parser, result_t *result);

