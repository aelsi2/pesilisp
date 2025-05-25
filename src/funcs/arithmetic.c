#include "arithmetic.h"
#include "utils.h"

static result_t lisp_int_add(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_null_term(list);

    intval_t result = 0;
    for (int i = 0; i < arg_count(list); i++) {
        ensure_integer(list, i);
        result += obj_get_int(list.array[i]);
    }
    obj_list_free(&list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_sub(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_at_least(list, 1);
    ensure_null_term(list);

    ensure_integer(list, 0);
    intval_t result = obj_get_int(list.array[0]);
    if (is_unary(list)) {
        result *= -1;
    }
    for (int i = 1; i < arg_count(list); i++) {
        ensure_integer(list, i);
        result -= obj_get_int(list.array[i]);
    }
    obj_list_free(&list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_mul(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_null_term(list);

    intval_t result = 1;
    for (int i = 0; i < arg_count(list); i++) {
        ensure_integer(list, i);
        result *= obj_get_int(list.array[i]);
    }
    obj_list_free(&list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_div(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_at_least(list, 1);
    ensure_null_term(list);

    ensure_integer(list, 0);
    intval_t result = obj_get_int(list.array[0]);
    if (is_unary(list)) {
        result = 1 / result;
    }
    for (int i = 1; i < arg_count(list); i++) {
        ensure_integer(list, i);
        result /= obj_get_int(list.array[i]);
    }
    obj_list_free(&list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_mod(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_exactly(list, 2);
    ensure_null_term(list);

    ensure_integer(list, 0);
    ensure_integer(list, 1);
    intval_t dividend = obj_get_int(list.array[0]);
    intval_t modulo = obj_get_int(list.array[1]);
    intval_t result = dividend % modulo;
    if (result > 0 != modulo > 0) {
        result += modulo;
    }
    obj_list_free(&list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_rem(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_exactly(list, 2);
    ensure_null_term(list);

    ensure_integer(list, 0);
    ensure_integer(list, 1);
    intval_t dividend = obj_get_int(list.array[0]);
    intval_t modulo = obj_get_int(list.array[1]);
    intval_t result = dividend % modulo;
    obj_list_free(&list);

    return result_success(obj_make_int(result));
}

static result_t lisp_int_eq(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_at_least(list, 1);
    ensure_null_term(list);

    ensure_integer(list, 0);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < arg_count(list); i++) {
        ensure_integer(list, i);
        if (obj_get_int(list.array[i]) != value) {
            result = false;
            break;
        }
    }
    obj_list_free(&list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_lt(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_at_least(list, 1);
    ensure_null_term(list);

    ensure_integer(list, 0);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < arg_count(list); i++) {
        ensure_integer(list, i);
        intval_t new_val = obj_get_int(list.array[i]);
        if (new_val <= value) {
            result = false;
            break;
        }
        value = new_val;
    }
    obj_list_free(&list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_gt(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_at_least(list, 1);
    ensure_null_term(list);

    ensure_integer(list, 0);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < arg_count(list); i++) {
        ensure_integer(list, i);
        intval_t new_val = obj_get_int(list.array[i]);
        if (new_val >= value) {
            result = false;
            break;
        }
        value = new_val;
    }
    obj_list_free(&list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_le(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_at_least(list, 1);
    ensure_null_term(list);

    ensure_integer(list, 0);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < arg_count(list); i++) {
        ensure_integer(list, i);
        intval_t new_val = obj_get_int(list.array[i]);
        if (new_val < value) {
            result = false;
            break;
        }
        value = new_val;
    }
    obj_list_free(&list);

    return result_success(result ? T : NIL);
}

static result_t lisp_int_ge(object_t *func, env_t *env, object_t *args) {
    read_args(list, args, env);
    args_eval_all(list, env);
    ensure_args_at_least(list, 1);
    ensure_null_term(list);

    ensure_integer(list, 0);
    intval_t value = obj_get_int(list.array[0]);
    bool result = true;
    for (int i = 1; i < arg_count(list); i++) {
        ensure_integer(list, i);
        intval_t new_val = obj_get_int(list.array[i]);
        if (new_val > value) {
            result = false;
            break;
        }
        value = new_val;
    }
    obj_list_free(&list);

    return result_success(result ? T : NIL);
}

void env_load_arithmetic(env_t *env) {
    env_register(env, "+", lisp_int_add);
    env_register(env, "-", lisp_int_sub);
    env_register(env, "*", lisp_int_mul);
    env_register(env, "/", lisp_int_div);
    env_register(env, "mod", lisp_int_mod);
    env_register(env, "rem", lisp_int_rem);
    env_register(env, "=", lisp_int_eq);
    env_register(env, "<", lisp_int_lt);
    env_register(env, ">", lisp_int_gt);
    env_register(env, "<=", lisp_int_le);
    env_register(env, ">=", lisp_int_ge);
}
