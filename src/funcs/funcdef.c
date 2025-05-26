#include "func.h"
#include "modules.h"
#include "symbol.h"
#include "utils.h"
#include <stdlib.h>

static result_t lisp_defun(object_t *func, object_t *args, env_t *env,
                           bool *dirty) {
    read_args(list, args, env);
    ensure_args_exactly(list, 3);

    if (!obj_is_sym(list.array[0])) {
        return result_error(NULL);
    }
    const char *name = obj_get_sym(list.array[0]);

    if (!obj_is_list(list.array[1])) {
        return result_error(NULL);
    }
    read_args(arg_spec, list.array[1], env);
    const char **arg_names = malloc(sizeof(char *) * arg_spec.count);
    for (int i = 0; i < arg_spec.count; i++) {
        if (!obj_is_sym(arg_spec.array[i])) {
            return result_error(NULL);
        }
        arg_names[i] = obj_get_sym(arg_spec.array[i]);
    }
    object_t *value = list.array[2];

    object_t *result =
        obj_make_lisp_func(name, arg_spec.count, arg_names, value, env);
    env_define(env, name, result);
    
    free(arg_names);
    free_args(arg_spec);
    free_args(list);

    return result_success(result);
}

static result_t lisp_lambda(object_t *func, object_t *args, env_t *env,
                           bool *dirty) {
    read_args(list, args, env);
    ensure_args_exactly(list, 2);

    if (!obj_is_list(list.array[0])) {
        return result_error(NULL);
    }
    read_args(arg_spec, list.array[0], env);
    const char **arg_names = malloc(sizeof(char *) * arg_spec.count);
    for (int i = 0; i < arg_spec.count; i++) {
        if (!obj_is_sym(arg_spec.array[i])) {
            return result_error(NULL);
        }
        arg_names[i] = obj_get_sym(arg_spec.array[i]);
    }
    object_t *value = list.array[1];

    object_t *result =
        obj_make_lisp_func(NULL, arg_spec.count, arg_names, value, env);
    
    free(arg_names);
    free_args(arg_spec);
    free_args(list);

    return result_success(result);
}

void env_load_funcdef(env_t *env) {
    env_register(env, "DEFUN", lisp_defun);
    env_register(env, "LAMBDA", lisp_lambda);
}
