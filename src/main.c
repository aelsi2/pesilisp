#include "data/environment.h"
#include "data/object.h"
#include "funcs/modules.h"
#include "parser.h"
#include "tty.h"
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
    env_load_funcdef(env);
    env_load_arithmetic(env);
    env_load_boolean(env);
    env_load_lists(env);
    env_load_interactive(env);

    fprintf(stdout, "%s", start_message);

    int line_num = 0;
    parser_t *parser = parser_new(stdin, "stdin");
    int prompt_number = 0;
    while (true) {
        fprintf(stdout, "[%d]> ", ++prompt_number);

        result_t parse_result;
        if (!parser_read_object(parser, &parse_result)) {
            fputc('\n', stdout);
            break;
        }

        const location_t *location = parser_location(parser);
        if (line_num == location->line) {
            fputc('\n', stdout);
        }
        line_num = location->line;

        if (result_is_error(&parse_result)) {
            error_t *error = parse_result.error;
            begin_color(stderr, COLOR_RED);
            error_print(error, stderr);
            end_color(stderr);
            error_free(error);
            continue;
        }

        bool dirty;
        result_t eval_result = obj_eval(parse_result.object, env, &dirty);
        obj_unref(parse_result.object);
        if (!result_is_error(&eval_result)) {
            begin_color(stdout, COLOR_BLUE);
            obj_print(eval_result.object, stdout);
            end_color(stdout);
            fputc('\n', stdout);
            obj_unref(eval_result.object);
            continue;
        }

        error_t *error = eval_result.error;
        if (error && error->type == ERROR_EXIT) {
            result = error->exit_code;
            error_free(error);
            break;
        }
        begin_color(stderr, COLOR_RED);
        error_print(error, stderr);
        end_color(stderr);
        error_free(error);
        continue;
    }
    fprintf(stdout, "%s", exit_message);

    env_free(env);
    return result;
}
