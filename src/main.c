#include <stdio.h>
#include "data/object.h"
#include "data/primitives.h"
#include "data/cons.h"

int main() {
    object_t *integer = obj_make_int(10);
    object_t *cons1 = obj_cons(integer, NIL);
    object_t *cons2 = obj_cons(cons1, NIL);
    object_t *cons3 = obj_cons(NIL, cons2);
    obj_print(cons3, stdout);
    obj_unref(cons3);
    obj_unref(cons2);
    obj_unref(cons1);
    obj_unref(integer);
    return 0;
}
