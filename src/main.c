#include "data/environment.h"
#include "data/object.h"
#include "funcs/arithmetic.h"
#include "funcs/base.h"
#include "funcs/boolean.h"
#include "funcs/list.h"
#include "parser.h"
#include "interactive.h"
#include <stdio.h>
#include <stdlib.h>

const char *start_message =
    "Welcome to PesiLISP!\n"
    "Type (help) to list the standard functions or (quit) to leave.\n\n";
const char *exit_message = "Bye.\n";

int main() {
    int result = 0;

    env_t *env = env_new(NULL);
    env_load_base(env);
    env_load_arithmetic(env);
    env_load_boolean(env);
    env_load_list(env);
    env_load_interactive(env);

    parser_t *parser = parser_new(stdin, "stdin");

    fprintf(stdout, "%s", start_message);

    int prompt_number = 0;
    while (true) {
        fprintf(stdout, "[%d]> ", ++prompt_number);

        result_t parse_result;
        if (!parser_read_object(parser, &parse_result)) {
            fputc('\n', stdout);
            break;
        }
        if (result_is_error(&parse_result)) {
            error_t *error = parse_result.error;
            error_print(error, stderr);
            error_free(error);
            continue;
        }

        result_t eval_result = obj_eval(parse_result.object, env);
        obj_unref(parse_result.object);
        if (!result_is_error(&eval_result)) {
            obj_print(eval_result.object, stdout);
            fputc('\n', stdout);
            obj_unref(eval_result.object);
            continue;
        }

        error_t *error = eval_result.error;
        if (!error) {
            continue;
        }
        if (error->type == ERROR_EXIT) {
            result = error->exit_code;
            error_free(error);
            break;
        } else {
            error_print(error, stderr);
        }
        error_free(error);
        continue;
    }
    fprintf(stdout, "%s", exit_message);

    env_free(env);
    return result;
}
