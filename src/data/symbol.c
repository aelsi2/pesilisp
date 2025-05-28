#include "symbol.h"
#include "data/primitives.h"
#include "environment.h"
#include "type.h"
#include <stdlib.h>
#include <string.h>

static const char *error_format_undef_var = "Undefined variable: '%s'.";

static error_t *error_undef_var(const char *name) {
    return error_runtime(NULL, error_format_undef_var, name);
}

typedef struct {
    object_t base;
    char *name;
} symbol_t;

static result_t symbol_eval(object_t *obj, env_t *env, int *recursion_limit, bool *dirty) {
    symbol_t *sym = (symbol_t *)obj;
    if (!env_is_defined(env, sym->name)) {
        return result_error(error_undef_var(sym->name));
    }
    return result_success(env_get(env, sym->name));
}

static void symbol_print(const object_t *obj, FILE *file) {
    symbol_t *sym = (symbol_t *)obj;
    fwrite(sym->name, 1, strlen(sym->name), file);
}

static hash_t symbol_hash(const object_t *obj) {
    symbol_t *sym = (symbol_t *)obj;
    return hash_str(sym->name);
}

static bool symbol_equals(const object_t *obj, const object_t *other) {
    if (!obj_is_sym(other)) {
        return false;
    }
    return !strcmp(obj_get_sym(obj), obj_get_sym(other));
}

static void symbol_free(object_t *obj) {
    symbol_t *sym = (symbol_t *)obj;
    free(sym->name);
    obj_free_default(obj);
}

const obj_type_t TYPE_SYM = (obj_type_t){
    .name = "SYMBOL",
    .base = &TYPE_T,
    .size = sizeof(symbol_t),
    .eval = symbol_eval,
    .print = symbol_print,
    .hash = symbol_hash,
    .equals = symbol_equals,
    .free = symbol_free,
};

object_t *obj_make_sym(const char *name) {
    symbol_t *symbol = obj_alloc_default(&TYPE_SYM, false);
    symbol->name = malloc(strlen(name) + 1);
    strcpy(symbol->name, name);
    return &symbol->base;
}

const char *obj_get_sym(const object_t *obj) {
    if (!obj_is_sym(obj)) {
        return NULL;
    }
    symbol_t *sym = (symbol_t *)obj;
    return sym->name;
}
