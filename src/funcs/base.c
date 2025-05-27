#include "func_utils.h"
#include "modules.h"

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
    env_register(env, "QUOTE", lisp_quote);
    env_register(env, "EXIT", lisp_exit);
    env_register(env, "QUIT", lisp_exit);
}
