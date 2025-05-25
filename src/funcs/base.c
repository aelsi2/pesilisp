#include "base.h"
#include "funcs/utils.h"

static result_t lisp_quote(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    ensure_args_exactly(list, 1);
    ensure_null_term(list);
    object_t *result = obj_ref(list.array[0]);
    free_args(list);

    return result_success(result);
}

static result_t lisp_exit(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    ensure_args_between(list, 0, 1);
    args_eval_all(list, env);
    ensure_null_term(list);

    int exit_code = 0;
    if (arg_count(list) == 1) {
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
    env_register(env, "quote", lisp_quote);
    env_register(env, "exit", lisp_exit);
    env_register(env, "quit", lisp_exit);
}
