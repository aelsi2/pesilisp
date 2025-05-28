#include "func_utils.h"

#include <limits.h>

static const char *error_format_few_args_exact =
    "Too few arguments provided to function %s\n"
    "Expected: %d    Got: %d";

static const char *error_format_many_args_exact =
    "Too many arguments provided to function %s\n"
    "Expected: %d    Got: %d";

static const char *error_format_few_args_min =
    "Too few arguments provided to function %s\n"
    "Expected: %d or more    Got: %d";

static const char *error_format_few_args_range =
    "Too few arguments provided to function %s\n"
    "Expected: between %d and %d    Got: %d";

static const char *error_format_many_args_range =
    "Too many arguments provided to function %s\n"
    "Expected: between %d and %d    Got: %d";

static const char *error_format_wrong_type =
    "Argument %d provided to function %s does not have the right type.\n"
    "Expected: %s    Got: %s";

static const char *error_format_not_list =
    "Argument %d provided to function %s does not have the right type.\n"
    "Expected: LIST (CONS or NULL)    Got: %s";

error_t *error_wrong_arg_count(const char *name, int min, int max, int actual) {
    if (max == INT_MAX) {
        return error_usage(NULL, error_format_few_args_min, name, min, actual);
    }
    if (min == max) {
        if (actual < min) {
            return error_usage(NULL, error_format_few_args_exact, name, min,
                               actual);
        } else {
            return error_usage(NULL, error_format_many_args_exact, name, min,
                               actual);
        }
    } else {
        if (actual < min) {
            return error_usage(NULL, error_format_few_args_range, name, min,
                               max, actual);
        } else {
            return error_usage(NULL, error_format_many_args_range, name, min,
                               max, actual);
        }
    }
}

error_t *error_wrong_arg_type(const char *func_name, int arg_index,
                              const obj_type_t *expected,
                              const obj_type_t *actual) {

    return error_usage(NULL, error_format_wrong_type, arg_index + 1, func_name,
                       type_get_name(expected), type_get_name(actual));
}

error_t *error_arg_not_list(const char *func_name, int arg_index,
                            const obj_type_t *actual) {

    return error_usage(NULL, error_format_not_list, arg_index + 1, func_name,
                       type_get_name(actual));
}
