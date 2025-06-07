#pragma once

#include "data/cons.h"
#include "data/primitives.h"
#include "object.h"
#include <stdbool.h>

// Represents a list of LISP object references.
typedef struct {
    object_t **array;
    int count;
} obj_list_t;

// Checks if a LISP object is a list (CONS or NULL)
static inline bool obj_is_list(object_t *obj) {
    return obj_is_null(obj) || obj_is_cons(obj);
}

// Flattens a CONS-list into an array of LISP objects
obj_list_t obj_list_flatten(object_t *obj);

// Unflattens an array of LISP objects back into a CONS-list.
object_t *obj_list_unflatten(obj_list_t *list);

// Copies the contents of src to dst writing over its previous contents.
void obj_list_copy(obj_list_t *dst, const obj_list_t *src);

// Frees the contents of a flattened object list.
void obj_list_free(obj_list_t *list);
