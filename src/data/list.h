#pragma once

#include "object.h"
#include <stdbool.h>

typedef struct {
    object_t **array;
    int count;
} obj_list_t;

bool obj_get_list(object_t *obj, obj_list_t *list);

bool obj_list_eval_all(obj_list_t *list, env_t *env, error_t **error);

void obj_list_copy(obj_list_t *dst, const obj_list_t *src);

void obj_list_free(obj_list_t *list);
