#include "modules.h"
#include "utils.h"
#include "data/cons.h"

static result_t lisp_cons(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    ensure_args_exactly(list, 2);
    args_eval_all(list, env);

    object_t *result = obj_cons(list.array[0], list.array[1]);
    free_args(list);

    return result_success(result);
}

static result_t lisp_car(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    ensure_args_exactly(list, 1);
    args_eval_all(list, env);

    if (!obj_is_list(list.array[0])) {
        free_args(list);
        return result_error(NULL);
    }
    object_t *result = obj_car(list.array[0]);
    free_args(list);

    return result_success(result);
}

static result_t lisp_cdr(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    ensure_args_exactly(list, 1);
    args_eval_all(list, env);

    if (!obj_is_list(list.array[0])) {
        free_args(list);
        return result_error(NULL);
    }
    object_t *result = obj_cdr(list.array[0]);
    free_args(list);

    return result_success(result);
}

static result_t lisp_list(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    object_t *result = obj_list_unflatten(&list);
    free_args(list);

    return result_success(result);
}

void env_load_lists(env_t *env) {
    env_register(env, "CONS", lisp_cons);
    env_register(env, "CAR", lisp_car);
    env_register(env, "CDR", lisp_cdr);
    env_register(env, "FIRST", lisp_car);
    env_register(env, "REST", lisp_cdr);
    env_register(env, "LIST", lisp_list);
}
