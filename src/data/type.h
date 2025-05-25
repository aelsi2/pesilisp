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
    bool is_primitive;
    result_t (*eval)(object_t *object, env_t *env);
    void (*print)(const object_t *object, FILE *file);
    hash_t (*hash)(const object_t *object);
    void (*free)(object_t *object);
};

struct object_s {
    const obj_type_t *type;
    int ref_count;
};

bool type_subtype_of(const obj_type_t* type, const obj_type_t* super);

void *obj_alloc_default(const obj_type_t *const type);

void obj_free_default(object_t *object);

result_t obj_eval_self(object_t *object, env_t *env);

void obj_print_default(const object_t *object, FILE *file);

hash_t obj_hash_default(const object_t *object);

void obj_free_noop(object_t *object);

