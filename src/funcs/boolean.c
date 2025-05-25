#include "boolean.h"
#include "funcs/utils.h"

static result_t lisp_bool_or(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    arg_eval(list, env, arg_count(list));
    ensure_null_term(list);

    object_t *result = NIL;
    for (int i = 0; i < arg_count(list); i++) {
        arg_eval(list, env, i);
        if (!obj_is_null(list.array[i])) {
            result = obj_ref(list.array[i]);
            break;
        }
    }
    free_args(list);

    return result_success(result);
}

static result_t lisp_bool_and(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    arg_eval(list, env, arg_count(list));
    ensure_null_term(list);

    object_t *result = T;
    for (int i = 0; i < arg_count(list); i++) {
        arg_eval(list, env, i);
        obj_unref(result);
        result = obj_ref(list.array[i]);
        if (obj_is_null(list.array[i])) {
            break;
        }
    }
    free_args(list);

    return result_success(result);
}

static result_t lisp_bool_not(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    ensure_args_exactly(list, 1);
    args_eval_all(list, env);
    ensure_null_term(list);

    object_t *result = obj_is_null(list.array[0]) ? T : NIL;
    free_args(list);

    return result_success(result);
}

static result_t lisp_if(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    ensure_args_between(list, 2, 3);
    arg_eval(list, env, arg_count(list));
    ensure_null_term(list);

    arg_eval(list, env, 0);
    object_t *result;
    if (obj_is_null(list.array[0])) {
        if (arg_count(list) == 2) {
            result = NIL;
        } else {
            arg_eval(list, env, 2);
            result = obj_ref(list.array[2]);
        }
    } else {
        arg_eval(list, env, 1);
        result = obj_ref(list.array[1]);
    }
    free_args(list);

    return result_success(result);
}

void env_load_boolean(env_t *env) {
    env_define(env, "T", T);
    env_define(env, "NIL", NIL);
    env_register(env, "or", lisp_bool_or);
    env_register(env, "and", lisp_bool_and);
    env_register(env, "not", lisp_bool_not);
    env_register(env, "null", lisp_bool_not);
    env_register(env, "if", lisp_if);
}
