#include "parser.h"
#include "data/cons.h"
#include "data/object.h"
#include "data/primitives.h"
#include "data/symbol.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static result_t parse_object(const char **str);

static char peek(const char **str) {
    return **str;
}

static char consume(const char **str) {
    return *((*str)++);
}

static void skip_whitespace(const char **str) {
    while (isspace(**str)) {
        *str += 1;
    }
}

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

static result_t parse_atom(const char **str) {
    size_t capacity = 16;
    size_t count = 0;
    char *name = malloc(capacity);
    while (isatom(peek(str))) {
        name[count++] = consume(str);
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

static result_t parse_list_tail(const char **str) {
    result_t car_res = parse_object(str);
    if (result_is_error(&car_res)) {
        return car_res;
    }
    object_t *car = car_res.object;
    object_t *cdr;
    skip_whitespace(str);
    char ch = peek(str);
    if (ch == '.') {
        consume(str);
        result_t cdr_res = parse_object(str);
        if (result_is_error(&cdr_res)) {
            obj_unref(car);
            return cdr_res;
        }
        cdr = cdr_res.object;
        skip_whitespace(str);
        if (peek(str) != ')') {
            obj_unref(car);
            obj_unref(cdr);
            return result_error(NULL);
        }
        consume(str);
    } else if (ch == ')') {
        consume(str);
        cdr = NIL;
    } else {
        result_t cdr_res = parse_list_tail(str);
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

static result_t parse_list(const char **str) {
    consume(str);
    skip_whitespace(str);
    char ch = peek(str);
    if (ch == ')') {
        consume(str);
        return result_success(NIL);
    }
    return parse_list_tail(str);
}

static result_t parse_quote(const char **str) {
    consume(str);
    skip_whitespace(str);
    result_t result = parse_object(str);
    if (result_is_error(&result)) {
        return result;
    }
    object_t *symbol = obj_make_sym("quote");
    object_t *cons_inner = obj_cons(result.object, NIL);
    object_t *cons_outer = obj_cons(symbol, cons_inner);
    obj_unref(cons_inner);
    obj_unref(result.object);
    obj_unref(symbol);
    return result_success(cons_outer);
}

static result_t parse_object(const char **str) {
    skip_whitespace(str);
    char ch = peek(str);
    if (ch == '(') {
        return parse_list(str);
    } else if (ch == '\'') {
        return parse_quote(str);
    } else if (isatom(ch)) {
        return parse_atom(str);
    } else {
        return result_error(NULL);
    }
}

result_t parse(const char *str) {
    return parse_object(&str);
}
