#include "func.h"
#include "data/cache.h"
#include "data/list.h"
#include "data/object.h"
#include "data/primitives.h"
#include "environment.h"
#include "func_utils.h"
#include "type.h"
#include <stdlib.h>
#include <string.h>

static const char *error_format_recursion_limit =
    "Recursion limit reached while trying to evaluate function '%s'.";

typedef struct {
    object_t base;
    char *name;
    lisp_callback_t *callback;
} func_t;

void func_free(object_t *obj) {
    func_t *func = (func_t *)obj;
    free(func->name);
    obj_free_default(obj);
}

void func_print(const object_t *obj, FILE *file) {
    func_t *func = (func_t *)obj;
    fprintf(file, "#<FUNCTION %s>", func->name);
}

const obj_type_t TYPE_FUNC = (obj_type_t){
    .name = "FUNCTION",
    .base = &TYPE_T,
    .size = sizeof(func_t),
    .eval = obj_eval_self,
    .print = func_print,
    .hash = obj_hash_default,
    .equals = obj_equals_default,
    .free = func_free,
};

typedef struct {
    func_t base;
    int arg_count;
    char **args;
    env_t *captured_env;
    object_t *value;
    cache_t *cache;
} lisp_func_t;

static void lisp_func_free(object_t *obj) {
    lisp_func_t *func = (lisp_func_t *)obj;
    for (int i = 0; i < func->arg_count; i++) {
        free(func->args[i]);
    }
    free(func->args);
    free(func->base.name);
    cache_free(func->cache);
    env_free(func->captured_env);
    obj_unref(func->value);
    obj_free_default(obj);
}

static const obj_type_t TYPE_LISP_FUNC = (obj_type_t){
    .name = "FUNCTION",
    .base = &TYPE_FUNC,
    .size = sizeof(lisp_func_t),
    .eval = obj_eval_self,
    .print = func_print,
    .hash = obj_hash_default,
    .equals = obj_equals_default,
    .free = lisp_func_free,
};

const char *obj_func_name(object_t *object) {
    if (!obj_is_func(object)) {
        return NULL;
    }
    func_t *func = (func_t *)object;
    return func->name;
}

static const char *func_name_default = "(anonymous)";

object_t *obj_make_native_func(const char *name, lisp_callback_t *callback) {
    func_t *func = obj_alloc_default(&TYPE_FUNC, false);
    func->callback = callback;
    if (!name) {
        name = func_name_default;
    }
    func->name = malloc(strlen(name) + 1);
    strcpy(func->name, name);
    return (object_t *)func;
}

static bool should_cache_args(obj_list_t *args) {
    for (int i = 0; i < args->count; i++) {
        if (obj_is_mutable(args->array[i])) {
            return false;
        }
    }
    return true;
}

static bool should_cache_result(object_t *result) {
    return !obj_is_mutable(result);
}

static result_t lisp_func_call(object_t *object, object_t *args, env_t *env,
                               int *recursion_depth, bool *dirty) {
    lisp_func_t *func = (lisp_func_t *)object;

    list_begin(list, args);
    ensure_args_exactly(func, list, func->arg_count);
    args_eval_all(list, env, recursion_depth, dirty);

    bool args_cache = should_cache_args(&list);
    object_t *cached_result = NIL;
    if (args_cache && cache_try_get(func->cache, &list, &cached_result)) {
        obj_list_free(&list);
        return result_success(cached_result);
    }

    env_t *exec_env = env_new(func->captured_env);
    for (int i = 0; i < func->arg_count; i++) {
        env_define(exec_env, func->args[i], list.array[i]);
    }
    if (func->base.name != NULL) {
        env_define(exec_env, func->base.name, object);
    }

    bool result_dirty = false;
    if (*recursion_depth <= 0) {
        list_end(list);
        env_free(exec_env);
        error_t *error =
            error_runtime(NULL, error_format_recursion_limit, func->base.name);
        return result_error(error);
    }
    (*recursion_depth)--;
    result_t result =
        obj_eval(func->value, exec_env, recursion_depth, &result_dirty);
    (*recursion_depth)++;
    bool result_cache = !result_dirty && !result_is_error(&result) &&
                        should_cache_result(result.object);
    if (args_cache && result_cache) {
        cache_remember(func->cache, &list, result.object);
    }
    if (result_dirty) {
        *dirty = true;
    }
    list_end(list);
    env_free(exec_env);
    return result;
}

object_t *obj_make_lisp_func(const char *name, int arg_count, const char **args,
                             object_t *value, env_t *environment) {
    lisp_func_t *func = obj_alloc_default(&TYPE_LISP_FUNC, true);

    if (!name) {
        name = func_name_default;
    }
    func->base.name = malloc(strlen(name) + 1);
    strcpy(func->base.name, name);
    func->captured_env = env_capture(environment);
    func->value = obj_ref(value);
    func->base.callback = lisp_func_call;
    func->cache = cache_new();

    func->arg_count = arg_count;
    func->args = malloc(sizeof(char *) * func->arg_count);
    for (int i = 0; i < func->arg_count; i++) {
        func->args[i] = malloc(strlen(args[i]) + 1);
        strcpy(func->args[i], args[i]);
    }

    return (object_t *)func;
}

result_t obj_call_func(object_t *object, object_t *args, env_t *env,
                       int *recursion_depth, bool *dirty) {
    if (!obj_is_func(object)) {
        return result_success(NIL);
    }
    func_t *func = (func_t *)object;
    return func->callback(object, args, env, recursion_depth, dirty);
}
