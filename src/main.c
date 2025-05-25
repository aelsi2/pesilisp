#include "data/cons.h"
#include "data/environment.h"
#include "data/func.h"
#include "data/list.h"
#include "data/object.h"
#include "data/primitives.h"
#include "data/symbol.h"
#include <stdio.h>

result_t lisp_add(object_t *func, env_t *env, object_t *args) {
    intval_t result = 0;

    obj_list_t list;
    if (!obj_get_list(args, &list)) {
        return result_error(NULL);
    }
    error_t *error;
    if (!obj_list_eval_all(&list, env, &error)) {
        obj_list_free(&list);
        return result_error(error);
    }

    for (int i = 0; i < list.count; i++) {
        if (!obj_is_int(list.array[i])) {
            obj_list_free(&list);
            return result_error(NULL);
        }
        result += obj_get_int(list.array[i]);
    }
    obj_list_free(&list);

    return result_success(obj_make_int(result));
}

int main() {
    object_t *add_func = obj_make_native_func(lisp_add);
    env_t *env = env_new(NULL);
    env_define(env, "+", add_func);
    add_func = obj_unref(add_func);

    object_t *symbol = obj_make_sym("+");
    object_t *arg1 = obj_make_int(3);
    object_t *arg2 = obj_make_int(7);

    object_t *args1 = obj_cons(arg2, NIL);
    object_t *args2 = obj_cons(arg1, args1);
    object_t *expr = obj_cons(symbol, args2);
    args2 = obj_unref(args2);
    args1 = obj_unref(args1);
    arg2 = obj_unref(arg2);
    arg1 = obj_unref(arg1);
    symbol = obj_unref(symbol);

    object_t *result = obj_eval(expr, env).object;

    obj_print(expr, stdout);
    fputc('\n', stdout);
    obj_print(result, stdout);
    fputc('\n', stdout);

    obj_unref(result);
    obj_unref(expr);
    env_free(env);

    return 0;
}
