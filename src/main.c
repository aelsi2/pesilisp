#include "data/environment.h"
#include "data/object.h"
#include "parser.h"
#include "stdlib/modules.h"
#include "tty.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *start_message =
    "Welcome to PesiLISP!\n"
    "Type (help) to list the standard functions or (quit) to leave.\n\n";
static const char *exit_message = "Bye.\n";
static const char *help_message =
    "PesiLISP - A perfectly simple LISP interpreter.\n"
    "\n"
    "Usage: pesilisp [-h | --help | lispfile]\n"
    "Options:\n"
    "-h --help : print this message\n"
    "Arguments:\n"
    "lispfile  : path to the script to execute\n"
    "\n"
    "Runs in REPL (interactive) mode when no parameters are specified.\n";
static const char *file_error_message = "Failed to open file %s\n";

static int run_repl(parser_t *parser, env_t *env) {
    int result = 0;
    fprintf(stdout, "%s", start_message);

    int line_num = 0;
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
            begin_color(stdout, COLOR_RED);
            error_print(error, stdout);
            end_color(stdout);
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
        } else {
            begin_color(stdout, COLOR_RED);
            error_print(error, stdout);
            end_color(stdout);
            error_free(error);
        }
    }
    fprintf(stdout, "%s", exit_message);

    return result;
}

static int run_normal(parser_t *parser, env_t *env) {
    int result = 0;

    while (true) {
        result_t parse_result;
        if (!parser_read_object(parser, &parse_result)) {
            break;
        }

        if (result_is_error(&parse_result)) {
            error_t *error = parse_result.error;
            begin_color(stderr, COLOR_RED);
            error_print(error, stderr);
            end_color(stderr);
            error_free(error);
            result = 1;
            break;
        }

        bool dirty;
        result_t eval_result = obj_eval(parse_result.object, env, &dirty);
        obj_unref(parse_result.object);

        if (result_is_error(&eval_result)) {
            error_t *error = eval_result.error;
            if (error && error->type == ERROR_EXIT) {
                result = error->exit_code;
            } else {
                result = 1;
                begin_color(stderr, COLOR_RED);
                error_print(error, stderr);
                end_color(stderr);
            }
            error_free(error);
            break;
        }
        obj_unref(eval_result.object);
    }
    return result;
}

env_t *make_env(void) {
    env_t *env = env_new(NULL);
    env_load_base(env);
    env_load_control(env);
    env_load_funcdef(env);
    env_load_arithmetic(env);
    env_load_boolean(env);
    env_load_lists(env);
    env_load_io(env);
    env_load_interactive(env);
    return env;
}

int main(int argc, char **argv) {
    int result = 0;

    if (argc <= 1) {
        parser_t *parser = parser_default();
        env_t *env = make_env();
        result = run_repl(parser, env);
        env_free(env);
    } else if (argc == 2 && argv[1][0] != '-') {
        const char *file_name = argv[1];
        FILE *file = fopen(file_name, "r");
        if (file) {
            parser_t *parser = parser_new(file, file_name);
            env_t *env = make_env();
            result = run_normal(parser, env);
            env_free(env);
            parser_free(parser);
            fclose(file);
        } else {
            begin_color(stderr, COLOR_RED);
            fprintf(stderr, file_error_message, file_name);
            end_color(stderr);
            result = 1;
        }
    } else {
        fprintf(stdout, "%s", help_message);
    }

    return result;
}
