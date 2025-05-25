#include "list.h"
#include "cons.h"
#include "primitives.h"
#include <stdlib.h>

bool obj_list_next(object_t **list, object_t **element) {
    if (obj_is_cons(*list)) {
        object_t *car = obj_car(*list);
        object_t *cdr = obj_cdr(*list);
        obj_unref(*list);
        obj_unref(*element);
        *element = car;
        *list = cdr;
        return true;
    } else {
        obj_unref(*list);
        obj_unref(*element);
        *list = NIL;
        *element = NIL;
        return false;
    }
}

bool obj_get_list(object_t *obj, obj_list_t *list) {
    int length = 0;
    object_t *len_obj = obj_ref(obj);
    while (obj_is_cons(len_obj)) {
        object_t *cdr = obj_cdr(len_obj);
        obj_unref(len_obj);
        len_obj = cdr;
        length++;
    }
    if (!obj_is_null(len_obj)) {
        obj_unref(len_obj);
        return false;
    }
    obj_unref(len_obj);

    list->count = length;
    list->array = malloc(sizeof(object_t *) * length);
    int index = 0;
    object_t *val_obj = obj_ref(obj);
    for (int i = 0; i < length; i++) {
        list->array[i] = obj_car(val_obj);
        object_t *cdr = obj_cdr(val_obj);
        obj_unref(val_obj);
        val_obj = cdr;
    }
    return true;
}

bool obj_list_eval_all(obj_list_t *list, env_t *env, error_t **error) {
    for (int i = 0; i < list->count; i++) {
        result_t eval_res = obj_eval(list->array[i], env);
        if (result_is_error(&eval_res)) {
            *error = eval_res.error;
            return false;
        }
        obj_unref(list->array[i]);
        list->array[i] = eval_res.object;
    }
    return true;
}

void obj_list_copy(obj_list_t *dst, const obj_list_t *src) {
    dst->count = src->count;
    dst->array = malloc(sizeof(object_t *) * dst->count);
    for (int i = 0; i < dst->count; i++) {
        dst->array[i] = obj_ref(src->array[i]);
    }
}

void obj_list_free(obj_list_t *list) {
    for (int i = 0; i < list->count; i++) {
        obj_unref(list->array[i]);
    }
    free(list->array);
    list->array = NULL;
    list->count = 0;
}
