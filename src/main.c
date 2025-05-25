#include "data/cons.h"
#include "data/environment.h"
#include "data/func.h"
#include "data/object.h"
#include "funcs/arithmetic.h"
#include "funcs/boolean.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

result_t lisp_quote(object_t *func, env_t *env, object_t *args) {
    return result_success(obj_car(args));
}

int main() {
    object_t *quote_func = obj_make_native_func(lisp_quote);
    env_t *env = env_new(NULL);
    env_load_arithmetic(env);
    env_load_boolean(env);

    env_define(env, "quote", quote_func);
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
