#pragma once

#include "object.h"
#include <stdbool.h>

extern const obj_type_t TYPE_T;
extern const obj_type_t TYPE_NULL;
extern const obj_type_t TYPE_INT;

// Represents a value stored in a LISP integer.
typedef long long intval_t;

// The T LISP value.
extern object_t *const T;

// The NIL LISP value.
extern object_t *const NIL;

// Checks if the object is a LISP integer.
inline static bool obj_is_int(const object_t *object) {
    return obj_get_type(object) == &TYPE_INT;
}

// Creates a new LISP integer with the specified value.
object_t *obj_make_int(intval_t value);

// Gets the value of the LISP integer.
// Returns 0 if the value is not an integer.
intval_t obj_get_int(const object_t *object);

// Checks if the object is a NIL value.
inline static bool obj_is_null(const object_t *object) {
    return obj_of_type(object, &TYPE_NULL);
}

