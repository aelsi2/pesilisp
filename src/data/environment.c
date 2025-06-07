#include "data/environment.h"
#include "data/object.h"
#include "data/primitives.h"
#include "hash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define LOAD_FACTOR 0.75
#define INITIAL_CAPACITY 16

typedef struct {
    char *key;
    object_t *value;
} kvp_t;

struct env_s {
    kvp_t *cells;
    size_t capacity;
    size_t count;
    env_t *parent;
};

env_t *env_new(env_t *parent) {
    env_t *env = malloc(sizeof(env_t));
    env->count = 0;
    env->capacity = INITIAL_CAPACITY;
    env->cells = calloc(env->capacity, sizeof(kvp_t));
    env->parent = parent;
    return env;
}

static kvp_t *env_find_cell(const env_t *env, const char *name) {
    size_t initial_index = hash_str(name) % env->capacity;
    size_t index = initial_index;
    while (true) {
        kvp_t *kvp = &env->cells[index];
        index = (index + 1) % env->capacity;
        if (!kvp->key) {
            return kvp;
        }
        if (!strcmp(kvp->key, name)) {
            return kvp;
        }
        if (index == initial_index) {
            return kvp;
        }
    }
}

bool env_is_defined(const env_t *env, const char *name) {
    kvp_t *cell = env_find_cell(env, name);
    if (cell->key) {
        return true;
    }
    if (env->parent) {
        return env_is_defined(env->parent, name);
    }
    return false;
}

static void env_grow(env_t *env) {
    size_t old_capacity = env->capacity;
    kvp_t *old_cells = env->cells;
    if (old_capacity >= INITIAL_CAPACITY) {
        env->capacity = old_capacity * 2;
    } else { 
        env->capacity = INITIAL_CAPACITY * 2;
    }
    env->cells = calloc(env->capacity, sizeof(kvp_t));
    for (int i = 0; i < old_capacity; i++) {
        kvp_t *old_cell = &old_cells[i];
        if (!old_cell->key) {
            continue;
        }
        kvp_t *new_cell = env_find_cell(env, old_cell->key);
        *new_cell = *old_cell;
    }
    free(old_cells);
}

void env_define(env_t *env, const char *name, object_t *value) {
    kvp_t *cell = env_find_cell(env, name);
    if (cell->key) {
        obj_unref(cell->value);
        cell->value = obj_ref(value);
        return;
    }
    if (++env->count >= env->capacity * LOAD_FACTOR) {
        env_grow(env);
        cell = env_find_cell(env, name);
    }
    char *name_copy = malloc(strlen(name) + 1);
    strcpy(name_copy, name);
    cell->key = name_copy;
    cell->value = obj_ref(value);
}

object_t *env_get(const env_t *env, const char *name) {
    kvp_t *cell = env_find_cell(env, name);
    if (cell->key) {
        return obj_ref(cell->value);
    } if (env->parent){
        return env_get(env->parent, name);
    }else {
        return NIL;
    }
}

static void env_add_all(env_t *dest, const env_t *src) {
    if (src->parent != NULL) {
        env_add_all(dest, src->parent);
    }
    for (int i = 0; i < src->capacity; i++) {
        kvp_t *cell = &src->cells[i];
        if (!cell->key) {
            continue;
        }
        env_define(dest, cell->key, cell->value);
    }
}

env_t *env_copy(const env_t *env) {
    env_t *copy = env_new(NULL);
    env_add_all(copy, env);
    return copy;
}

void env_free(env_t *env) {
    for (int i = 0; i < env->capacity; i++) {
        if (!env->cells[i].key) {
            continue;
        }
        free(env->cells[i].key);
        obj_unref(env->cells[i].value);
    }
    free(env->cells);
    free(env);
}

