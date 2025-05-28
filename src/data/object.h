#pragma once

#include "error.h"
#include <stdbool.h>
#include <stdio.h>
#include "hash.h"

typedef struct env_s env_t;
typedef struct obj_type_s obj_type_t;

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

inline static bool result_is_error(result_t *result) {
    return result->object == NULL;
}

// Checks if the LISP object is mmutable.
bool obj_is_mutable(const object_t *object);

// Gets the most specific type of the LISP object.
const obj_type_t *obj_get_type(const object_t *object);

// Checks if the LISP object is of the specified type.
bool obj_of_type(const object_t *object, const obj_type_t *type);

bool type_subtype_of(const obj_type_t* type, const obj_type_t* super);

const char *type_get_name(const obj_type_t* type);

// Evaluates the LISP object and returns a new object or an error.
result_t obj_eval(object_t *object, env_t *env, int *recursion_depth, bool *dirty);

// Prints the LISP object to the file.
void obj_print(const object_t *object, FILE *file);

// Hashes the LISP object.
hash_t obj_hash(const object_t *object);

// Checks if two LISP objects are equal.
bool obj_equals(const object_t *object, const object_t *other);

// Creates a new reference to the LISP object and
// increments its reference count.
// Returns the new reference.
object_t *obj_ref(object_t *object);

// Releases the reference to the LISP object and
// decrements its reference count.
// Returns the LISP NIL value.
object_t *obj_unref(object_t *object);
