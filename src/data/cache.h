#pragma once

#include "list.h"
#include <stdbool.h>

typedef struct cache_s cache_t;

cache_t *cache_new(void);
void cache_free(cache_t *cache);

bool cache_try_get(cache_t *cache, obj_list_t *args, object_t **value);
void cache_store(cache_t *cache, obj_list_t *args, object_t *value);
