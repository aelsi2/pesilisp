#include "parser.h"
#include "data/cons.h"
#include "data/object.h"
#include "data/primitives.h"
#include "data/symbol.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_UNINIT (-1)
#define CHAR_EOF (-2)

struct parser_s {
    FILE *file;
    char *file_name;
    int current_value;
    int line;
    int column;
};

parser_t *parser_new(FILE *file, const char* file_name) {
    parser_t *parser = malloc(sizeof(parser_t));
    parser->file = file;
    parser->line = 0;
    parser->column = 0;
    parser->current_value = CHAR_UNINIT;
    if (file_name) {
        parser->file_name = malloc(strlen(file_name) + 1);
        strcpy(parser->file_name, file_name);
    } else {
        parser->file_name = NULL;
    }
    return parser;
}

void parser_free(parser_t *parser) {
    free(parser->file_name);
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
        parser->line++;
        parser->column = 0;
    } else {
        parser->column++;
    }
    parser->current_value = parser_read_char(parser);
    return value;
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

static result_t parser_parse_object(parser_t *parser);

static bool isatom(char ch) {
    if (isspace(ch)) {
        return false;
    }
    switch (ch) {
    case '(':
    case ')':
    case '\'':
    case '\0':
        return false;
    }
    return true;
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

static result_t parser_parse_list_tail(parser_t *parser) {
    result_t car_res = parser_parse_object(parser);
    if (result_is_error(&car_res)) {
        return car_res;
    }
    object_t *car = car_res.object;
    object_t *cdr;
    parser_move_to_next(parser);
    char ch = parser_peek(parser);
    if (ch == '.') {
        parser_consume(parser);
        parser_move_to_next(parser);
        result_t cdr_res = parser_parse_object(parser);
        if (result_is_error(&cdr_res)) {
            obj_unref(car);
            return cdr_res;
        }
        cdr = cdr_res.object;
        parser_move_to_next(parser);
        if (parser_peek(parser) != ')') {
            obj_unref(car);
            obj_unref(cdr);
            return result_error(NULL);
        }
        parser_consume(parser);
    } else if (ch == ')') {
        parser_consume(parser);
        cdr = NIL;
    } else {
        result_t cdr_res = parser_parse_list_tail(parser);
        if (result_is_error(&cdr_res)) {
            obj_unref(car);
            return cdr_res;
        }
        cdr = cdr_res.object;
    }
    object_t *cons = obj_cons(car, cdr);
    obj_unref(car);
    obj_unref(cdr);
    return result_success(cons);
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

static result_t parser_parse_object(parser_t *parser) {
    char ch = parser_peek(parser);
    if (ch == '(') {
        return parser_parse_list(parser);
    } else if (ch == '\'') {
        return parser_parse_quote(parser);
    } else if (isatom(ch)) {
        return parser_parse_atom(parser);
    } else {
        return result_error(NULL);
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

