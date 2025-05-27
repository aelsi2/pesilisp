#include "func_utils.h"

#include <limits.h>

static const char *func_name_anon = "(lambda)";

static const char *error_format_few_args_exact =
    "Not enough arguments provided to function %s.\n"
    "Got: %d. Expected: %d.";

static const char *error_format_many_args_exact =
    "Too many arguments provided to function %s.\n"
    "Got: %d. Expected: %d.";

static const char *error_format_few_args_min =
    "Not enough arguments provided to function %s.\n"
    "Got: %d. Expected: %d or more.";

static const char *error_format_few_args_range =
    "Not enough arguments provided to function %s.\n"
    "Got: %d. Expected: between %d and %d.";

static const char *error_format_many_args_range =
    "Too many arguments provided to function %s.\n"
    "Got: %d. Expected: between %d and %d.";

static const char *error_format_wrong_type =
    "Argument %d provided to function %s does not have the right type.\n"
    "Got: %s. Expected: %s.";

static const char *error_format_not_list =
    "Argument %d provided to function %s does not have the right type.\n"
    "Got: %s. Expected: LIST (CONS or NULL).";

error_t *error_wrong_arg_count(const char *name, int min, int max, int actual) {
    if (!name) {
        name = func_name_anon;
    }
    if (max == INT_MAX) {
        return error_usage(NULL, error_format_few_args_min, name, actual, min);
    }
    if (min == max) {
        if (actual < min) {
            return error_usage(NULL, error_format_few_args_exact, name, actual,
                               min);
        } else {
            return error_usage(NULL, error_format_many_args_exact, name, actual,
                               min);
        }
    } else {
        if (actual < min) {
            return error_usage(NULL, error_format_few_args_range, name, actual,
                               min, max);
        } else {
            return error_usage(NULL, error_format_many_args_range, name, actual,
                               min, max);
        }
    }
}

error_t *error_wrong_arg_type(const char *func_name, int arg_index,
                              const obj_type_t *expected,
                              const obj_type_t *actual) {

    return error_usage(NULL, error_format_wrong_type, arg_index + 1, func_name,
                       type_get_name(actual), type_get_name(expected));
}

error_t *error_arg_not_list(const char *func_name, int arg_index,
                            const obj_type_t *actual) {
    
    return error_usage(NULL, error_format_not_list, arg_index + 1, func_name,
                       type_get_name(actual));
}
