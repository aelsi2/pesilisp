#pragma once

#include "object.h"
#include <stdbool.h>

extern const obj_type_t TYPE_FUNC;

typedef result_t lisp_callback_t(object_t *func, int arg_count, object_t **args);

static inline bool obj_is_func(const object_t *object) {
    return obj_of_type(object, &TYPE_FUNC);
}

object_t *obj_make_native_func(lisp_callback_t *callback);

object_t *obj_make_lisp_func(int arg_count, const char** args, object_t *value,
                             env_t *environment);

result_t obj_call_func(object_t *func, int arg_count, object_t **args);
