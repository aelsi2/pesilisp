#pragma once

#include "data/object.h"
#include <stdbool.h>

extern const obj_type_t TYPE_CONS;

inline static bool obj_is_cons(const object_t *obj) {
    return obj_of_type(obj, &TYPE_CONS);
}

object_t *obj_cons(object_t *car, object_t *cdr);

object_t *obj_car(object_t *obj);

object_t *obj_cdr(object_t *obj);

bool obj_list_next(object_t **list, object_t **element);

int obj_list_length(object_t *obj);

