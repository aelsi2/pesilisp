
#include "data/object.h"
#include "data/symbol.h"
#include "func_utils.h"
#include "modules.h"

const char *error_format_not_supported = "Function %s is not supported on the current platform.";

#ifdef _WIN32
#include "windows.h"
#include "winuser.h"

static result_t lisp_message(object_t *func, object_t *args, env_t *env,
                          int recursion_limit, bool *dirty) {
    list_begin(list, args);
    ensure_args_exactly(func, list, 1);
    ensure_type(func, list, 0, &TYPE_SYM);
    const char *message = obj_get_sym(list.array[0]);
    int result = MessageBox(NULL, message, "PesiLISP", MB_OK | MB_ICONWARNING);
    *dirty = true;
    list_end(list);
    return result_success(result == IDOK ? T : NIL);
}
#else
static result_t lisp_message(object_t *func, object_t *args, env_t *env,
                          int recursion_limit, bool *dirty) {
    const char *func_name = obj_func_name((object_t *)func);
    error_t *error = error_runtime(NULL, error_format_not_supported, func_name);
    return result_error(error);
}
#endif

void env_load_gui(env_t *env) {
    env_register_func(env, "MESSAGE", lisp_message);
}
