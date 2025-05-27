#include "data/func.h"
#include "data/symbol.h"
#include "func_utils.h"
#include "modules.h"
#include <stdlib.h>

static const char *error_format_defun_name_not_symbol =
    "The first argument in a DEFUN form must be a symbol.\nGot an instance of "
    "%s instead.";

static const char *error_format_defun_bad_arg_spec =
    "The second argument in a DEFUN form must be a list of symbols.";

static const char *error_format_lambda_bad_arg_spec =
    "The first argument in a LAMBDA form must be a list of symbols.";

static result_t lisp_defun(object_t *func, object_t *args, env_t *env,
                           bool *dirty) {
    read_args(list, args);
    ensure_args_exactly(func, list, 3);

    if (!obj_is_sym(list.array[0])) {
        const char *type_name = type_get_name(obj_get_type(list.array[0]));
        error_t *error =
            error_usage(NULL, error_format_defun_name_not_symbol, type_name);
        free_args(list);
        return result_error(error);
    }
    const char *name = obj_get_sym(list.array[0]);

    if (!obj_is_list(list.array[1])) {
        error_t *error = error_usage(NULL, error_format_defun_bad_arg_spec);
        free_args(list);
        return result_error(error);
    }
    read_args(arg_spec, list.array[1]);
    const char **arg_names = malloc(sizeof(char *) * arg_spec.count);
    for (int i = 0; i < arg_spec.count; i++) {
        if (!obj_is_sym(arg_spec.array[i])) {
            error_t *error = error_usage(NULL, error_format_defun_bad_arg_spec);
            free(arg_names);
            free_args(list);
            free_args(arg_spec);
            return result_error(error);
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
    read_args(list, args);
    ensure_args_exactly(func, list, 2);

    if (!obj_is_list(list.array[0])) {
        error_t *error = error_usage(NULL, error_format_lambda_bad_arg_spec);
        free_args(list);
        return result_error(error);
    }
    read_args(arg_spec, list.array[0]);
    const char **arg_names = malloc(sizeof(char *) * arg_spec.count);
    for (int i = 0; i < arg_spec.count; i++) {
        if (!obj_is_sym(arg_spec.array[i])) {
            error_t *error =
                error_usage(NULL, error_format_lambda_bad_arg_spec);
            free(arg_names);
            free_args(list);
            free_args(arg_spec);
            return result_error(error);
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
