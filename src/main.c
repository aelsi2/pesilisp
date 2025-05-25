#include "data/cons.h"
#include "data/environment.h"
#include "data/func.h"
#include "data/list.h"
#include "data/object.h"
#include "data/primitives.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

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

result_t lisp_quote(object_t *func, env_t *env, object_t *args) {
    return result_success(obj_car(args));
}

int main() {
    object_t *add_func = obj_make_native_func(lisp_add);
    object_t *quote_func = obj_make_native_func(lisp_quote);
    env_t *env = env_new(NULL);
    env_define(env, "+", add_func);
    env_define(env, "quote", quote_func);
    env_define(env, "T", T);
    env_define(env, "NIL", NIL);
    add_func = obj_unref(add_func);
    quote_func = obj_unref(quote_func);

    fprintf(stdout, "Welcome to PesiLISP\nType (help) to list the standard functions.\n");
    char *buffer = NULL;
    size_t size;
    while (true) {
        fprintf(stdout, ">> ");
        getline(&buffer, &size, stdin);
        if (feof(stdin)) {
            fprintf(stdout, "\n");
            break;
        }
        result_t parse_result = parse(buffer);
        if (result_is_error(&parse_result)) {
            continue;
        }
        result_t eval_result = obj_eval(parse_result.object, env);
        obj_unref(parse_result.object);

        if (result_is_error(&eval_result)) {
            continue;
        }

        obj_print(eval_result.object, stdout);
        fputc('\n', stdout);
        obj_unref(eval_result.object);
    }
    free(buffer);
    env_free(env);

    return 0;
}
