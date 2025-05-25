#pragma once

#include "object.h"
#include <stdbool.h>

typedef struct {
    object_t **array;
    int count;
} obj_list_t;

// Flattens a CONS-list into an array of LISP objects (including the terminating NULL or other value).
// Overwrites the list parameter.
obj_list_t obj_flatten(object_t *obj);

bool obj_list_eval_all(obj_list_t *list, env_t *env, error_t **error);

void obj_list_copy(obj_list_t *dst, const obj_list_t *src);

void obj_list_free(obj_list_t *list);
