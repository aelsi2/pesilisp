#pragma once

#include "error.h"
#include <stdbool.h>
#include <stdio.h>

// A special LISP object ref_count value that disables
// reference counting.
#define REFCOUNT_OFF (-1)

// Represents a LISP type identifier.
typedef enum {
    TYPE_NIL = 0,
    TYPE_T = 1,
    TYPE_INT = 2,
    TYPE_CONS = 3,
    TYPE_SYM = 4,
    TYPE_FUNC = 5
} obj_type_t;

// Represents a LISP object.
typedef struct object_s object_t;

// Represents the result of an object evaluation.
typedef struct {
    object_t *object;
    error_t *error;
} result_t;

// Constructs an evaluation result from an object.
inline static result_t result_success(object_t *object) {
    return (result_t){
        .object = object,
        .error = NULL,
    };
}

// Constructs an evaluation result from an error.
inline static result_t result_error(error_t *error) {
    return (result_t){
        .object = NULL,
        .error = error,
    };
}

// Gets the type id of the LISP object.
obj_type_t obj_type(const object_t *object);

// Evaluates the LISP object and returns a new object or an error.
result_t obj_eval(object_t *object);

// Prints the LISP object to the file.
void obj_print(const object_t *object, FILE *file);

// Creates a new reference to the LISP object and
// increments its reference count.
// Returns the new reference.
object_t *obj_ref(object_t *object);

// Releases the reference to the LISP object and
// decrements its reference count.
// Returns the LISP NIL value.
object_t *obj_unref(object_t *object);

