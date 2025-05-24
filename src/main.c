#include "data/cons.h"
#include "data/environment.h"
#include "data/object.h"
#include "data/primitives.h"
#include "data/symbol.h"
#include <stdio.h>

int main() {
    object_t *integer = obj_make_int(10);
    object_t *cons1 = obj_cons(integer, NIL);
    object_t *cons2 = obj_cons(cons1, NIL);
    object_t *cons3 = obj_cons(NIL, cons2);
    env_t *env = env_new(NULL);
    env_set(env, "cons3", cons3);
    obj_unref(cons3);
    obj_unref(cons2);
    obj_unref(cons1);
    obj_unref(integer);
    object_t *sym = obj_make_sym("cons3");
    object_t *cons_ref = obj_eval(sym, env).object;

    obj_print(sym, stdout);
    fputc('\n', stdout);

    obj_print(cons_ref, stdout);
    fputc('\n', stdout);

    fprintf(stdout, "%llx\n", obj_hash(cons_ref));

    obj_unref(sym);
    obj_unref(cons_ref);
    env_unref(env);
    return 0;
}
