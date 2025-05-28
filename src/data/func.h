#pragma once

#include "object.h"
#include <stdbool.h>

extern const obj_type_t TYPE_FUNC;

typedef result_t lisp_callback_t(object_t *func, object_t *args, env_t *env,
                                 int recursion_limit, bool *dirty);

static inline bool obj_is_func(const object_t *object) {
    return obj_of_type(object, &TYPE_FUNC);
}

const char *obj_func_name(object_t *object);

object_t *obj_make_native_func(const char *name, lisp_callback_t *callback);

object_t *obj_make_lisp_func(const char *name, int arg_count, const char **args,
                             object_t *value, env_t *environment);

result_t obj_call_func(object_t *func, object_t *args, env_t *env,
                       int recursion_limit, bool *dirty);
