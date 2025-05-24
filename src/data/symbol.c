#include "symbol.h"
#include "environment.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    object_t base;
    char *name;
} symbol_t;

static result_t symbol_eval(object_t *obj, env_t *env) {
    symbol_t *sym = (symbol_t *)obj;
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

static void symbol_free(object_t *obj) {
    symbol_t *sym = (symbol_t *)obj;
    free(sym->name);
    obj_free_default(obj);
}

static obj_vtable_t *symbol_vtable = &(obj_vtable_t){
    .type = TYPE_SYM,
    .eval = symbol_eval,
    .print = symbol_print,
    .hash = symbol_hash,
    .free = symbol_free,
};

object_t *obj_make_sym(const char *name) {
    symbol_t *symbol = obj_alloc_default(symbol_vtable, sizeof(symbol_t));
    symbol->name = malloc(strlen(name) + 1);
    strcpy(symbol->name, name);
    return &symbol->base;
}

const char *obj_get_sym(object_t *obj) {
    if (!obj_is_sym(obj)) {
        return NULL;
    }
    symbol_t *sym = (symbol_t *)obj;
    return sym->name;
}
