#include "data/environment.h"
#include "data/object.h"
#include "funcs/arithmetic.h"
#include "funcs/base.h"
#include "funcs/boolean.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    int result = 0;
    
    env_t *env = env_new(NULL);
    env_load_base(env);
    env_load_arithmetic(env);
    env_load_boolean(env);

    fprintf(stdout, "Welcome to PesiLISP\nType (help) to list the standard functions or (quit) to leave.\n");
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
            error_t *error = parse_result.error;
            error_print(error, stderr);
            error_free(error);
            continue;
        }
        result_t eval_result = obj_eval(parse_result.object, env);
        obj_unref(parse_result.object);

        if (result_is_error(&eval_result)) {
            error_t *error = eval_result.error;
            if (error && error->type == ERROR_EXIT) {
                result = error->exit_code;
                error_free(error);
                break;
            }
            error_print(error, stderr);
            error_free(error);
            continue;
        }

        obj_print(eval_result.object, stdout);
        fputc('\n', stdout);
        obj_unref(eval_result.object);
    }
    free(buffer);
    env_free(env);

    return result;
}
