#include "data/object.h"
#include "func_utils.h"
#include "modules.h"

static const char *help_message = "\n"
                           "Common:\n"
                           "  QUOTE, LET, LET*,\n"
                           "  EVAL, APPLY, NULL, EQ\n"
                           "Flow control:\n"
                           "  IF, COND, EXIT (QUIT)\n"
                           "Integer arithmetic:\n"
                           "  +, -, *, /, MOD, REM,\n"
                           "  =, /=, <, >, <=, >=\n"
                           "Boolean algebra:\n"
                           "  AND, OR, NOT\n"
                           "List processing:\n"
                           "  CONS, LIST,\n"
                           "  CAR (FIRST), CDR (REST)\n"
                           "Function definition:\n"
                           "  DEFUN, LAMBDA\n"
                           "Input/Output:\n"
                           "  PRINT, READ\n"
                           "Interactive:\n"
                           "  HELP\n"
                           "\n";

static result_t lisp_help(object_t *func, object_t *args, env_t *env,
                          int recursion_limit, bool *dirty) {
    list_begin(list, args);
    ensure_args_exactly(func, list, 0);
    list_end(list);
    *dirty = true;
    fprintf(stdout, "%s", help_message);
    return result_success(NIL);
}

void env_load_interactive(env_t *env) {
    env_register_func(env, "HELP", lisp_help);
}
