#include "func.h"
#include "data/object.h"
#include "data/primitives.h"
#include "environment.h"
#include "type.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    object_t base;
    lisp_callback_t *callback;
} func_t;

const obj_type_t TYPE_FUNC = (obj_type_t){
    .name = "FUNC",
    .base = &TYPE_T,
    .size = sizeof(func_t),
    .eval = obj_eval_self,
    .print = obj_print_default,
    .hash = obj_hash_default,
    .free = obj_free_default,
};

typedef struct {
    func_t base;
    int arg_count;
    char **args;
    env_t *captured_env;
    object_t *value;
} lisp_func_t;

static void lisp_func_free(object_t *obj) {
    lisp_func_t *func = (lisp_func_t *)obj;
    for (int i = 0; i < func->arg_count; i++) {
        free(func->args[i]);
    }
    env_unref(func->captured_env);
    obj_unref(func->value);
    obj_free_default(obj);
}

static const obj_type_t TYPE_LISP_FUNC = (obj_type_t){
    .name = "FUNC",
    .base = &TYPE_FUNC,
    .size = sizeof(lisp_func_t),
    .eval = obj_eval_self,
    .print = obj_print_default,
    .hash = obj_hash_default,
    .free = lisp_func_free,
};

object_t *obj_make_native_func(lisp_callback_t *callback) {
    func_t *func = obj_alloc_default(&TYPE_FUNC);
    func->callback = callback;
    return (object_t *)func;
}

static result_t lisp_func_call(object_t *object, int arg_count,
                               object_t **args) {
    lisp_func_t *func = (lisp_func_t *)object;
    if (arg_count != func->arg_count) {
    }

    env_t *env = env_new(func->captured_env);
    for (int i = 0; i < func->arg_count; i++) {
        env_define(env, func->args[i], args[i]);
    }
    result_t result = obj_eval(func->value, env);
    env_unref(env);
    return result;
}

object_t *obj_make_lisp_func(int arg_count, const char **args, object_t *value,
                             env_t *environment) {
    lisp_func_t *func = obj_alloc_default(&TYPE_LISP_FUNC);

    func->arg_count = arg_count;
    func->args = malloc(sizeof(char *) * func->arg_count);
    func->captured_env = env_ref(environment);
    func->value = obj_ref(value);
    func->base.callback = lisp_func_call;

    for (int i = 0; i < func->arg_count; i++) {
        func->args[i] = malloc(strlen(args[i]) + 1);
        strcpy(func->args[i], args[i]);
    }

    return (object_t *)func;
}

result_t obj_call_func(object_t *object, int arg_count, object_t **args) {
    if (!obj_is_func(object)) {
    }
    func_t *func = (func_t *)object;
    return func->callback(object, arg_count, args);
}
