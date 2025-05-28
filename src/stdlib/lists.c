#include "data/cons.h"
#include "func_utils.h"
#include "modules.h"

static result_t lisp_cons(object_t *func, object_t *args, env_t *env,
                          int *recursion_depth, bool *dirty) {
    list_begin(list, args);
    ensure_args_exactly(func, list, 2);
    args_eval_all(list, env, recursion_depth, dirty);

    object_t *result = obj_cons(list.array[0], list.array[1]);
    list_end(list);

    return result_success(result);
}

static result_t lisp_car(object_t *func, object_t *args, env_t *env,
                         int *recursion_depth, bool *dirty) {
    list_begin(list, args);
    ensure_args_exactly(func, list, 1);
    args_eval_all(list, env, recursion_depth, dirty);
    ensure_list(func, list, 0);

    object_t *result = obj_car(list.array[0]);
    list_end(list);

    return result_success(result);
}

static result_t lisp_cdr(object_t *func, object_t *args, env_t *env,
                         int *recursion_depth, bool *dirty) {
    list_begin(list, args);
    ensure_args_exactly(func, list, 1);
    args_eval_all(list, env, recursion_depth, dirty);
    ensure_list(func, list, 0);

    object_t *result = obj_cdr(list.array[0]);
    list_end(list);

    return result_success(result);
}

static result_t lisp_list(object_t *func, object_t *args, env_t *env,
                          int *recursion_depth, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_depth, dirty);
    object_t *result = obj_list_unflatten(&list);
    list_end(list);

    return result_success(result);
}

void env_load_lists(env_t *env) {
    env_register_func(env, "CONS", lisp_cons);
    env_register_func(env, "CAR", lisp_car);
    env_register_func(env, "CDR", lisp_cdr);
    env_register_func(env, "FIRST", lisp_car);
    env_register_func(env, "REST", lisp_cdr);
    env_register_func(env, "LIST", lisp_list);
}
