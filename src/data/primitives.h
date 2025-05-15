#pragma once

#include "object.h"
#include <stdbool.h>

// Represents a value stored in a LISP integer.
typedef long long intval_t;

// The T LISP value.
extern object_t *const T;

// The NIL LISP value.
extern object_t *const NIL;

// Checks if the object is a LISP integer.
inline static bool is_integer(object_t *object) {
    return obj_type(object) == TYPE_INT;
}

// Creates a new LISP integer with the specified value.
object_t *obj_make_int(intval_t value);

// Gets the value of the LISP integer.
// Returns 0 if the value is not an integer.
intval_t obj_get_int(object_t *object);

// Creates the LISP representation of the boolean value.
inline static object_t *obj_make_bool(bool value) {
    return value ? T : NIL;
}

// Creates the LISP representation of the boolean value.
inline static bool obj_get_bool(object_t *object) {
    return obj_type(object) != TYPE_NIL;
}

