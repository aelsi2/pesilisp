#include "data/object.h"
#include "func_utils.h"
#include "modules.h"

const char *help_message = "\n"
                           "Common functions:\n"
                           "  QUOTE, NULL, EXIT (QUIT)\n"
                           "Integer arithmetic:\n"
                           "  +, -, *, /, MOD, REM,\n"
                           "  =, <, >, <=, >=\n"
                           "Boolean algebra:\n"
                           "  IF, AND, OR, NOT\n"
                           "List processing:\n"
                           "  CONS, LIST,\n"
                           "  CAR (FIRST), CDR (REST)\n"
                           "Interactive:\n"
                           "  HELP\n"
                           "\n";

static result_t lisp_help(object_t *func, object_t *args, env_t *env,
                          bool *dirty) {
    *dirty = true;
    fprintf(stdout, "%s", help_message);
    return result_success(NIL);
}

void env_load_interactive(env_t *env) {
    env_register_func(env, "HELP", lisp_help);
}
