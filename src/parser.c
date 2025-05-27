#include "parser.h"
#include "data/cons.h"
#include "data/object.h"
#include "data/primitives.h"
#include "data/symbol.h"
#include "location.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_UNINIT (-128)
#define CHAR_EOF (-129)

static const char *error_format_unex_eof =
    "Unexpected EOF. Did you forget a ')'?";

static const char *error_format_unex_char = "Unexpected character: '%c'.";

static const char *error_format_no_dot_list_end =
    "Dotted list not terminated with a ')'.";

static const char *error_format_bad_char_range =
    "One or more invalid chatacters encountered.";

static const char *error_format_invalid_dot = "Dot ('.') is not allowed here.";

struct parser_s {
    FILE *file;
    location_t location;
    int current_value;
};

parser_t *parser_new(FILE *file, const char *file_name) {
    parser_t *parser = malloc(sizeof(parser_t));
    parser->file = file;
    parser->current_value = CHAR_UNINIT;
    location_init(&parser->location, file_name, 1, 1);
    return parser;
}

const location_t *parser_location(parser_t *parser) {
    return &parser->location;
}

void parser_free(parser_t *parser) {
    location_free(&parser->location);
    free(parser);
}

static int parser_read_char(parser_t *parser) {
    int value = fgetc(parser->file);
    return value == EOF ? CHAR_EOF : value;
}

static int parser_peek(parser_t *parser) {
    if (parser->current_value == CHAR_UNINIT) {
        parser->current_value = parser_read_char(parser);
    }
    return parser->current_value;
}

static int parser_consume(parser_t *parser) {
    if (parser->current_value == CHAR_UNINIT) {
        parser->current_value = parser_read_char(parser);
    }
    int value = parser->current_value;
    if (value == '\n') {
        location_next_line(&parser->location);
    } else {
        location_next_column(&parser->location);
    }
    parser->current_value = parser_read_char(parser);
    return value;
}

static bool isatom(int ch) {
    if (!isgraph(ch)) {
        return false;
    }
    switch (ch) {
    case '(':
    case ')':
    case '\'':
        return false;
    }
    return true;
}

static bool isstart(int ch) {
    if (isatom(ch)) {
        return true;
    }
    switch (ch) {
    case '(':
    case '\'':
        return true;
    }
    return false;
}

static bool isbad(int ch) {
    return !isspace(ch) && !isgraph(ch);
}

static void parser_move_to_next(parser_t *parser) {
    while (true) {
        int ch = parser_peek(parser);
        if (ch == ';') {
            parser_consume(parser);
            do {
                ch = parser_consume(parser);
            } while (ch != '\n' && ch != EOF);
        } else if (isspace(ch)) {
            do {
                parser_consume(parser);
                ch = parser_peek(parser);
            } while (isspace(ch) && ch != EOF);
        } else {
            break;
        }
    }
}

static error_t *error_dot_list_end(const location_t *location) {
    return error_syntax(location, error_format_no_dot_list_end);
}

static error_t *error_bad_char_range(const location_t *location) {
    return error_syntax(location, error_format_bad_char_range);
}

static error_t *error_invalid_dot(const location_t *location) {
    return error_syntax(location, error_format_invalid_dot);
}

static error_t *error_unexpected(const location_t *location, int ch) {
    error_t *error;
    if (ch == EOF) {
        error = error_syntax(location, error_format_unex_eof);
    } else {
        error = error_syntax(location, error_format_unex_char, ch);
    }
    return error;
}

static bool try_parse_int(const char *str, intval_t *result) {
    intval_t res = 0;
    bool negative = false;
    if (*str == '-') {
        negative = true;
        str++;
    }
    int index = 0;
    while (str[index]) {
        char ch = str[index++];
        if (isdigit(ch)) {
            res *= 10;
            res += ch - '0';
        } else {
            return false;
        }
    }
    if (negative) {
        res = -res;
    }
    *result = res;
    return index != 0;
}

static result_t parser_parse_atom(parser_t *parser) {
    size_t capacity = 16;
    size_t count = 0;
    char *name = malloc(capacity);
    while (isatom(parser_peek(parser))) {
        name[count++] = toupper(parser_consume(parser));
        if (count == capacity - 1) {
            capacity *= 2;
            name = realloc(name, capacity);
        }
    }
    name[count] = '\0';

    object_t *object;
    intval_t int_value;
    if (!strcmp(name, "T")) {
        object = T;
    } else if (!strcmp(name, "NIL")) {
        object = NIL;
    } else if (try_parse_int(name, &int_value)) {
        object = obj_make_int(int_value);
    } else {
        object = obj_make_sym(name);
    }
    free(name);
    return result_success(object);
}

static result_t parser_parse_object(parser_t *parser);

static result_t parser_parse_list_tail(parser_t *parser) {
    object_t *car = NIL;
    object_t *cdr = NIL;

    error_t *error = NULL;

    result_t car_res = parser_parse_object(parser);
    if (result_is_error(&car_res)) {
        error = car_res.error;
    } else {
        car = car_res.object;
    }

    parser_move_to_next(parser);
    char ch = parser_peek(parser);
    if (ch == '.') {
        parser_consume(parser);
        parser_move_to_next(parser);

        result_t cdr_res = parser_parse_object(parser);
        if (result_is_error(&cdr_res)) {
            if (error == NULL) {
                error = cdr_res.error;
            } else {
                error_free(cdr_res.error);
            }
        } else {
            cdr = cdr_res.object;
        }

        parser_move_to_next(parser);
        if (parser_peek(parser) == ')') {
            parser_consume(parser);
        } else {
            if (error == NULL) {
                error = error_dot_list_end(&parser->location);
            }
        }
    } else if (ch == ')') {
        parser_consume(parser);
        cdr = NIL;
    } else {
        result_t cdr_res = parser_parse_list_tail(parser);
        if (result_is_error(&cdr_res)) {
            if (error == NULL) {
                error = cdr_res.error;
            } else {
                error_free(cdr_res.error);
            }
        } else {
            cdr = cdr_res.object;
        }
    }
    if (error != NULL) {
        obj_unref(car);
        obj_unref(cdr);
        return result_error(error);
    } else {
        object_t *cons = obj_cons(car, cdr);
        obj_unref(car);
        obj_unref(cdr);
        return result_success(cons);
    }
}

static result_t parser_parse_list(parser_t *parser) {
    parser_consume(parser);
    parser_move_to_next(parser);
    char ch = parser_peek(parser);
    if (ch == ')') {
        parser_consume(parser);
        return result_success(NIL);
    }
    return parser_parse_list_tail(parser);
}

static result_t parser_parse_quote(parser_t *parser) {
    parser_consume(parser);
    parser_move_to_next(parser);
    result_t result = parser_parse_object(parser);
    if (result_is_error(&result)) {
        return result;
    }
    object_t *symbol = obj_make_sym("QUOTE");
    object_t *cons_inner = obj_cons(result.object, NIL);
    object_t *cons_outer = obj_cons(symbol, cons_inner);
    obj_unref(cons_inner);
    obj_unref(result.object);
    obj_unref(symbol);
    return result_success(cons_outer);
}

static result_t parser_parse_bad(parser_t *parser) {
    location_t start;
    location_copy_temp(&start, &parser->location);
    while (isbad(parser_peek(parser))) {
        parser_consume(parser);
    }
    return result_error(error_bad_char_range(&start));
}

static result_t parser_parse_object(parser_t *parser) {
    char ch = parser_peek(parser);
    if (ch == '.') {
        error_t *error = error_invalid_dot(&parser->location);
        parser_consume(parser);
        return result_error(error);
    } else if (ch == '(') {
        return parser_parse_list(parser);
    } else if (ch == '\'') {
        return parser_parse_quote(parser);
    } else if (isatom(ch)) {
        return parser_parse_atom(parser);
    } else if (isbad(ch)) {
        return parser_parse_bad(parser);
    } else {
        error_t *error = error_unexpected(&parser->location, ch);
        parser_consume(parser);
        return result_error(error);
    }
}

bool parser_read_object(parser_t *parser, result_t *result) {
    parser_move_to_next(parser);
    if (parser_peek(parser) == CHAR_EOF) {
        return false;
    }
    *result = parser_parse_object(parser);
    return true;
}
