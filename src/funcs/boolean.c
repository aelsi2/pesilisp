#include "boolean.h"
#include "funcs/utils.h"

static result_t lisp_bool_or(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    arg_eval(list, env, list.count);

    object_t *result = NIL;
    for (int i = 0; i < list.count; i++) {
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
    arg_eval(list, env, list.count);

    object_t *result = T;
    for (int i = 0; i < list.count; i++) {
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

    object_t *result = obj_is_null(list.array[0]) ? T : NIL;
    free_args(list);

    return result_success(result);
}

static result_t lisp_if(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    ensure_args_between(list, 2, 3);
    arg_eval(list, env, list.count);

    arg_eval(list, env, 0);
    object_t *result;
    if (obj_is_null(list.array[0])) {
        if (list.count == 2) {
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
    env_register(env, "OR", lisp_bool_or);
    env_register(env, "AND", lisp_bool_and);
    env_register(env, "NOT", lisp_bool_not);
    env_register(env, "NULL", lisp_bool_not);
    env_register(env, "IF", lisp_if);
}
