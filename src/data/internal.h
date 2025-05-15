#pragma once

#include "object.h"

typedef struct {
    obj_type_t type;
    result_t (*eval)(object_t *object);
    void (*print)(const object_t *object, FILE *file);
    void (*free)(object_t *object);
} obj_vtable_t;

struct object_s {
    const obj_vtable_t *vtable;
    int ref_count;
};


void *obj_alloc_default(obj_vtable_t *vtable, size_t size);

void obj_free_default(object_t *object);

result_t obj_eval_self(object_t *object);

void obj_free_noop(object_t *object);
