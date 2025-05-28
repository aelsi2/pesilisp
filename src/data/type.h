#pragma once

#include "object.h"
#include <stdbool.h>

// A special LISP object ref_count value that disables
// reference counting.
#define REFCOUNT_OFF (-1)

struct obj_type_s {
    char *name;
    const obj_type_t *base;
    size_t size;
    result_t (*eval)(object_t *object, env_t *env, int *recursion_limit, bool *dirty);
    void (*print)(const object_t *object, FILE *file);
    hash_t (*hash)(const object_t *object);
    bool (*equals)(const object_t *object, const object_t *other);
    void (*free)(object_t *object);
};

struct object_s {
    const obj_type_t *type;
    bool is_mutable;
    int ref_count;
};


void *obj_alloc_default(const obj_type_t *const type, bool is_ref_safe);

void obj_free_default(object_t *object);

result_t obj_eval_self(object_t *object, env_t *env, int *recursion_limit, bool *dirty);

void obj_print_default(const object_t *object, FILE *file);

hash_t obj_hash_default(const object_t *object);

void obj_free_noop(object_t *object);

bool obj_equals_default(const object_t *object, const object_t *other);

