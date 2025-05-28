#include "func_utils.h"
#include "modules.h"
#include "parser.h"

static const char *error_format_nothing_to_read = "Nothing to read.";

static result_t lisp_print(object_t *func, object_t *args, env_t *env,
                           int *recursion_depth, bool *dirty) {
    list_begin(list, args);
    ensure_args_exactly(func, list, 1);
    arg_eval(list, 0, env, recursion_depth, dirty);

    object_t *object = obj_ref(list.array[0]);
    obj_print(object, stdout);
    fputc('\n', stdout);
    *dirty = true;

    list_end(list);
    return result_success(object);
}

static result_t lisp_read(object_t *func, object_t *args, env_t *env,
                          int *recursion_depth, bool *dirty) {
    list_begin(list, args);
    ensure_args_exactly(func, list, 0);

    *dirty = true;
    result_t result;
    parser_t *parser = parser_default();
    if (!parser_read_object(parser, &result)) {
        list_end(list);
        return result_error(error_runtime(parser_location(parser),
                                          error_format_nothing_to_read));
    }

    list_end(list);
    return result;
}

void env_load_io(env_t *env) {
    env_register_func(env, "PRINT", lisp_print);
    env_register_func(env, "READ", lisp_read);
}
