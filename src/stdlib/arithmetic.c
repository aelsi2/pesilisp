#include "error.h"
#include "func_utils.h"
#include "modules.h"

static const char *error_format_divide_by_zero = "Division by zero.";

static error_t *error_divide_by_zero(void) {
    return error_runtime(NULL, error_format_divide_by_zero);
}

static result_t lisp_int_add(object_t *func, object_t *args, env_t *env,
                             int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);

    intval_t result = 0;
    for (int i = 0; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        result += obj_get_int(list.array[i]);
    }
    list_end(list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_sub(object_t *func, object_t *args, env_t *env,
                             int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_at_least(func, list, 1);

    ensure_type(func, list, 0, &TYPE_INT);
    intval_t result = obj_get_int(list.array[0]);
    if (list.count == 1) {
        result *= -1;
    }
    for (int i = 1; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        result -= obj_get_int(list.array[i]);
    }
    list_end(list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_mul(object_t *func, object_t *args, env_t *env,
                             int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);

    intval_t result = 1;
    for (int i = 0; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        result *= obj_get_int(list.array[i]);
    }
    list_end(list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_div(object_t *func, object_t *args, env_t *env,
                             int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_at_least(func, list, 1);

    ensure_type(func, list, 0, &TYPE_INT);
    intval_t result = obj_get_int(list.array[0]);
    if (list.count == 1) {
        if (result == 0) {
            list_end(list);
            return result_error(error_divide_by_zero());
        }
        result = 1 / result;
    }
    for (int i = 1; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        intval_t value = obj_get_int(list.array[i]);
        if (value == 0) {
            list_end(list);
            return result_error(error_divide_by_zero());
        }
        result /= value;
    }
    list_end(list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_mod(object_t *func, object_t *args, env_t *env,
                             int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_exactly(func, list, 2);

    ensure_type(func, list, 0, &TYPE_INT);
    ensure_type(func, list, 1, &TYPE_INT);
    intval_t dividend = obj_get_int(list.array[0]);
    intval_t modulo = obj_get_int(list.array[1]);
    if (modulo == 0) {
        list_end(list);
        return result_error(error_divide_by_zero());
    }
    intval_t result = dividend % modulo;
    if (result > 0 != modulo > 0) {
        result += modulo;
    }
    list_end(list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_rem(object_t *func, object_t *args, env_t *env,
                             int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_exactly(func, list, 2);

    ensure_type(func, list, 0, &TYPE_INT);
    ensure_type(func, list, 1, &TYPE_INT);
    intval_t dividend = obj_get_int(list.array[0]);
    intval_t modulo = obj_get_int(list.array[1]);
    if (modulo == 0) {
        list_end(list);
        return result_error(error_divide_by_zero());
    }
    intval_t result = dividend % modulo;
    list_end(list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_eq(object_t *func, object_t *args, env_t *env,
                            int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_at_least(func, list, 1);

    ensure_type(func, list, 0, &TYPE_INT);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        if (obj_get_int(list.array[i]) != value) {
            result = false;
            break;
        }
    }
    list_end(list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_neq(object_t *func, object_t *args, env_t *env,
                            int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_at_least(func, list, 1);

    bool result = true;
    for (int i = 0; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        intval_t value = obj_get_int(list.array[i]);
        for (int j = 0; j < i; j++) {
            intval_t other = obj_get_int(list.array[j]);
            if (value == other) {
                result = false;
                break;
            }
        }
    }
    list_end(list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_lt(object_t *func, object_t *args, env_t *env,
                            int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_at_least(func, list, 1);

    ensure_type(func, list, 0, &TYPE_INT);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        intval_t new_val = obj_get_int(list.array[i]);
        if (new_val <= value) {
            result = false;
            break;
        }
        value = new_val;
    }
    list_end(list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_gt(object_t *func, object_t *args, env_t *env,
                            int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_at_least(func, list, 1);

    ensure_type(func, list, 0, &TYPE_INT);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        intval_t new_val = obj_get_int(list.array[i]);
        if (new_val >= value) {
            result = false;
            break;
        }
        value = new_val;
    }
    list_end(list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_le(object_t *func, object_t *args, env_t *env,
                            int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_at_least(func, list, 1);

    ensure_type(func, list, 0, &TYPE_INT);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        intval_t new_val = obj_get_int(list.array[i]);
        if (new_val < value) {
            result = false;
            break;
        }
        value = new_val;
    }
    list_end(list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_ge(object_t *func, object_t *args, env_t *env,
                            int recursion_limit, bool *dirty) {
    list_begin(list, args);
    args_eval_all(list, env, recursion_limit, dirty);
    ensure_args_at_least(func, list, 1);

    ensure_type(func, list, 0, &TYPE_INT);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < list.count; i++) {
        ensure_type(func, list, i, &TYPE_INT);
        intval_t new_val = obj_get_int(list.array[i]);
        if (new_val > value) {
            result = false;
            break;
        }
        value = new_val;
    }
    list_end(list);

    return result_success(result ? T : NIL);
}

void env_load_arithmetic(env_t *env) {
    env_register_func(env, "+", lisp_int_add);
    env_register_func(env, "-", lisp_int_sub);
    env_register_func(env, "*", lisp_int_mul);
    env_register_func(env, "/", lisp_int_div);
    env_register_func(env, "MOD", lisp_int_mod);
    env_register_func(env, "REM", lisp_int_rem);
    env_register_func(env, "=", lisp_int_eq);
    env_register_func(env, "/=", lisp_int_neq);
    env_register_func(env, "<", lisp_int_lt);
    env_register_func(env, ">", lisp_int_gt);
    env_register_func(env, "<=", lisp_int_le);
    env_register_func(env, ">=", lisp_int_ge);
}
