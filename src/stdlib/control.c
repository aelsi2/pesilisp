#include "data/environment.h"
#include "func_utils.h"
#include "modules.h"

static const char *error_format_bad_cond_clause =
    "Each COND clause must be a list with at least 1 element";

static result_t read_cond_clause(object_t *clause, env_t *env,
                                 int *recursion_depth, bool *dirty,
                                 bool *match) {
    if (!obj_is_cons(clause)) {
        return result_error(error_usage(NULL, error_format_bad_cond_clause));
    }
    list_begin(list, clause);

    if (list.count == 1) {
        result_t result = obj_eval(list.array[0], env, recursion_depth, dirty);
        list_end(list);
        *match = true;
        return result;
    }

    result_t cond_result = obj_eval(list.array[0], env, recursion_depth, dirty);
    if (result_is_error(&cond_result)) {
        list_end(list);
        return cond_result;
    }
    if (obj_is_null(cond_result.object)) {
        list_end(list);
        *match = false;
        return cond_result;
    }
    obj_unref(cond_result.object);
    *match = true;

    object_t *result = NIL;
    for (int i = 1; i < list.count; i++) {
        obj_unref(result);
        result_t eval_res =
            obj_eval(list.array[i], env, recursion_depth, dirty);
        if (result_is_error(&eval_res)) {
            list_end(list);
            return eval_res;
        }
        result = eval_res.object;
    }
    list_end(list);

    return result_success(result);
}

static result_t lisp_cond(object_t *func, object_t *args, env_t *env,
                          int *recursion_depth, bool *dirty) {
    list_begin(list, args);

    bool match = false;
    object_t *result = NIL;
    for (int i = 0; i < list.count; i++) {
        obj_unref(result);
        result_t eval_res = read_cond_clause(list.array[i], env,
                                             recursion_depth, dirty, &match);
        if (result_is_error(&eval_res)) {
            list_end(list);
            return eval_res;
        }
        result = eval_res.object;
        if (match) {
            break;
        }
    }
    list_end(list);

    return result_success(result);
}

static result_t lisp_if(object_t *func, object_t *args, env_t *env,
                        int *recursion_depth, bool *dirty) {
    list_begin(list, args);
    ensure_args_between(func, list, 2, 3);

    arg_eval(list, 0, env, recursion_depth, dirty);
    object_t *result;
    if (obj_is_null(list.array[0])) {
        if (list.count == 2) {
            result = NIL;
        } else {
            arg_eval(list, 2, env, recursion_depth, dirty);
            result = obj_ref(list.array[2]);
        }
    } else {
        arg_eval(list, 1, env, recursion_depth, dirty);
        result = obj_ref(list.array[1]);
    }
    list_end(list);

    return result_success(result);
}

static result_t lisp_exit(object_t *func, object_t *args, env_t *env,
                          int *recursion_depth, bool *dirty) {
    list_begin(list, args);
    ensure_args_at_most(func, list, 1);
    args_eval_all(list, env, recursion_depth, dirty);

    int exit_code = 0;
    if (list.count == 1) {
        object_t *arg = list.array[0];
        if (obj_is_int(arg)) {
            exit_code = obj_get_int(arg);
        } else {
            exit_code = obj_is_null(arg) ? 0 : 1;
        }
    }
    list_end(list);

    return result_error(error_exit(exit_code));
}

void env_load_control(env_t *env) {
    env_register_func(env, "IF", lisp_if);
    env_register_func(env, "COND", lisp_cond);
    env_register_func(env, "EXIT", lisp_exit);
    env_register_func(env, "QUIT", lisp_exit);
}
