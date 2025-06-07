#include "list.h"
#include "cons.h"
#include <stdlib.h>

obj_list_t obj_list_flatten(object_t *obj) {
    obj_list_t list;
    size_t capacity = 16;
    list.count = 0;
    list.array = malloc(sizeof(object_t *) * capacity);

    obj = obj_ref(obj);
    while (obj_is_cons(obj)) {
        object_t *car = obj_car(obj);
        object_t *cdr = obj_cdr(obj);
        obj_unref(obj);
        obj = cdr;
        list.array[list.count++] = car;
        if (list.count == capacity - 1) {
            capacity *= 2;
            list.array = realloc(list.array, sizeof(object_t *) * capacity);
        }
    }
    list.array[list.count] = obj;
    return list;
}

object_t *obj_list_unflatten(obj_list_t *list) {
    object_t *result = NIL;
    for (int i = list->count - 1; i >= 0; i--) {
        object_t *new_res = obj_cons(list->array[i], result);
        obj_unref(result);
        result = new_res;
    }
    return result;
}

void obj_list_copy(obj_list_t *dst, const obj_list_t *src) {
    dst->count = src->count;
    dst->array = malloc(sizeof(object_t *) * (dst->count + 1));
    for (int i = 0; i < dst->count + 1; i++) {
        dst->array[i] = obj_ref(src->array[i]);
    }
}

void obj_list_free(obj_list_t *list) {
    for (int i = 0; i < list->count + 1; i++) {
        obj_unref(list->array[i]);
    }
    free(list->array);
    list->array = NULL;
    list->count = 0;
}
