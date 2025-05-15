#include <stdio.h>
#include "data/object.h"
#include "data/primitives.h"

int main() {
    obj_print(NIL, stdout);
    putc('\n', stdout);
    obj_print(T, stdout);
    putc('\n', stdout);
    printf("%d %d \n", obj_get_bool(T), obj_get_bool(NIL));
    object_t *obj = obj_make_int(256);
    obj_print(obj, stdout);
    putc('\n', stdout);
    obj_unref(obj);
    return 0;
}
