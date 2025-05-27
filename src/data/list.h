#pragma once

#include "data/cons.h"
#include "data/primitives.h"
#include "object.h"
#include <stdbool.h>

typedef struct {
    object_t **array;
    int count;
} obj_list_t;

// Checks if a LISP object is a list (a CONS cell or NIL)
static inline bool obj_is_list(object_t *obj) {
    return obj_is_null(obj) || obj_is_cons(obj);
}

// Flattens a CONS-list into an array of LISP objects
// Overwrites the list parameter.
obj_list_t obj_flatten(object_t *obj);

// Unflattens an array of LISP objects back into a CONS-list.
object_t *obj_list_unflatten(obj_list_t *list);

void obj_list_copy(obj_list_t *dst, const obj_list_t *src);

void obj_list_free(obj_list_t *list);
