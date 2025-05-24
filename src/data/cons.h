#pragma once

#include "data/object.h"
#include <stdbool.h>

inline static bool obj_is_cons(const object_t *obj) {
    return obj_type(obj) == TYPE_CONS;
}

object_t *obj_cons(object_t *car, object_t *cdr);

object_t *obj_car(object_t *obj);

object_t *obj_cdr(object_t *obj);

