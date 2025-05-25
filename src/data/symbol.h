#pragma once

#include "data/object.h"

extern const obj_type_t TYPE_SYM;

// Checks if the object is a LISP symbol.
inline static bool obj_is_sym(const object_t *object) {
    return obj_get_type(object) == &TYPE_SYM;
}

// Creates a new LISP symbol.
object_t *obj_make_sym(const char *name);

// Gets the name of the LISP symbol.
const char *obj_get_sym(const object_t *object);
