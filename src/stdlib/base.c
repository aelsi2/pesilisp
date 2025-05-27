#include "func_utils.h"
#include "modules.h"
#include "symbol.h"

static const char *error_format_let_not_list =
    "The first parameter of a LET form must be a list of definitions.\n"
    "Got an instance of %d instead.";
static const char *error_format_let_bad_def =
    "Bad LET variable definition.\n"
    "Each definition must be a list of a symbol and a value.";

static error_t *error_let_not_list(const obj_type_t *type) {
    return error_usage(NULL, error_format_let_not_list, type_get_name(type));
}

static error_t *error_bad_def(void) {
    return error_usage(NULL, error_format_let_bad_def);
}

static error_t *read_let_list(object_t *args, env_t *source, env_t *target,
                              bool *dirty) {
    if (!obj_is_list(args)) {
        return error_let_not_list(obj_get_type(args));
    }
    read_args(list, args);
    for (int i = 0; i < list.count; i++) {
        if (!obj_is_list(list.array[i])) {
            free_args(list);
            return error_bad_def();
        }
        read_args(def, list.array[i]);
        if (def.count != 2 || !obj_is_sym(def.array[0])) {
            free_args(def);
            free_args(list);
            return error_bad_def();
        }
        result_t eval_result = obj_eval(def.array[1], source, dirty);
        if (result_is_error(&eval_result)) {
            free_args(def);
            free_args(list);
            return eval_result.error;
        }
        env_define(target, obj_get_sym(def.array[0]), eval_result.object);
        obj_unref(eval_result.object);
        free_args(def);
    }
    free_args(list);
    return NULL;
}

static result_t lisp_let(object_t *func, object_t *args, env_t *env,
                         bool *dirty) {
    read_args(list, args);
    ensure_args_at_least(func, list, 1);
    env_t *child_env = env_new(env);

    error_t *error = read_let_list(list.array[0], env, child_env, dirty);
    if (error) {
        env_free(child_env);
        free_args(list);
        return result_error(error);
    }

    object_t *result = NIL;
    for (int i = 1; i < list.count; i++) {
        obj_unref(result);
        result_t eval_res = obj_eval(list.array[i], child_env, dirty);
        if (result_is_error(&eval_res)) {
            env_free(child_env);
            free_args(list);
            return eval_res;
        }
        result = eval_res.object;
    }

    env_free(child_env);
    free_args(list);
    return result_success(result);
}

static result_t lisp_let_star(object_t *func, object_t *args, env_t *env,
                         bool *dirty) {
    read_args(list, args);
    ensure_args_at_least(func, list, 1);
    env_t *child_env = env_new(env);

    error_t *error = read_let_list(list.array[0], child_env, child_env, dirty);
    if (error) {
        env_free(child_env);
        free_args(list);
        return result_error(error);
    }

    object_t *result = NIL;
    for (int i = 1; i < list.count; i++) {
        obj_unref(result);
        result_t eval_res = obj_eval(list.array[i], child_env, dirty);
        if (result_is_error(&eval_res)) {
            env_free(child_env);
            free_args(list);
            return eval_res;
        }
        result = eval_res.object;
    }

    env_free(child_env);
    free_args(list);
    return result_success(result);
}

static result_t lisp_eq(object_t *func, object_t *args, env_t *env,
                        bool *dirty) {
    read_args(list, args);
    ensure_args_exactly(func, list, 2);
    args_eval_all(list, env, dirty);

    bool are_equal = obj_equals(list.array[0], list.array[1]);
    object_t *result = are_equal ? T : NIL;
    free_args(list);

    return result_success(result);
}

static result_t lisp_quote(object_t *func, object_t *args, env_t *env,
                           bool *dirty) {
    read_args(list, args);
    ensure_args_exactly(func, list, 1);
    object_t *result = obj_ref(list.array[0]);
    free_args(list);

    return result_success(result);
}

static result_t lisp_exit(object_t *func, object_t *args, env_t *env,
                          bool *dirty) {
    read_args(list, args);
    ensure_args_at_most(func, list, 1);
    args_eval_all(list, env, dirty);

    int exit_code = 0;
    if (list.count == 1) {
        object_t *arg = list.array[0];
        if (obj_is_int(arg)) {
            exit_code = obj_get_int(arg);
        } else {
            exit_code = obj_is_null(arg) ? 0 : 1;
        }
    }
    free_args(list);

    return result_error(error_exit(exit_code));
}

void env_load_base(env_t *env) {
    env_register_func(env, "EQ", lisp_eq);
    env_register_func(env, "QUOTE", lisp_quote);
    env_register_func(env, "LET", lisp_let);
    env_register_func(env, "LET*", lisp_let_star);
    env_register_func(env, "EXIT", lisp_exit);
    env_register_func(env, "QUIT", lisp_exit);
}
