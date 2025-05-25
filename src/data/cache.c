#include "cache.h"
#include "data/primitives.h"
#include "data/type.h"
#include <stdlib.h>

#define LOAD_FACTOR 0.75
#define INITIAL_CAPACITY 16

typedef struct {
    obj_list_t key;
    object_t *value;
} kvp_t;

struct cache_s {
    kvp_t *cells;
    size_t capacity;
    size_t count;
};

cache_t *cache_new() {
    cache_t *cache = malloc(sizeof(cache_t));
    cache->count = 0;
    cache->capacity = INITIAL_CAPACITY;
    cache->cells = calloc(cache->capacity, sizeof(kvp_t));
    return cache;
}

void cache_free(cache_t *cache) {
    for (int i = 0; i < cache->capacity; i++) {
        if (!cache->cells[i].value) {
            continue;
        }
        obj_list_free(&cache->cells[i].key);
        obj_unref(cache->cells[i].value);
    }
    free(cache->cells);
    free(cache);
}

bool should_cache(obj_list_t *args) {
    for (int i = 0; i < args->count; i++) {
        if (!obj_get_type(args->array[i])->is_primitive) {
            return false;
        }
    }
    return true;
}

static size_t cache_get_index(const cache_t *cache, obj_list_t *args) {
    hash_t hash = hash_default;
    for (int i = 0; i < args->count; i++) {
        hash = hash_combine(hash, obj_hash(args->array[i]));
    }
    return hash % cache->capacity;
}

static bool obj_list_all_equal(const obj_list_t *list,
                               const obj_list_t *other) {
    if (list->count != other->count) {
        return false;
    }
    for (int i = 0; i < list->count; i++) {
        if (!obj_equals(list->array[i], other->array[i])) {
            return false;
        }
    }
    return true;
}

static kvp_t *cache_find_cell(const cache_t *cache, obj_list_t *args) {
    size_t initial_index = cache_get_index(cache, args);
    size_t index = initial_index;
    while (true) {
        kvp_t *kvp = &cache->cells[index];
        index = (index + 1) % cache->capacity;
        if (!kvp->value) {
            return kvp;
        }
        if (obj_list_all_equal(args, &kvp->key)) {
            return kvp;
        }
        if (index == initial_index) {
            return kvp;
        }
    }
}

bool cache_try_get(cache_t *cache, obj_list_t *args, object_t **value) {
    kvp_t *cell = cache_find_cell(cache, args);
    if (cell->value) {
        *value = obj_ref(cell->value);
        return true;
    } else {
        *value = NIL;
        return false;
    }
}

static void cache_grow(cache_t *cache) {
    size_t old_capacity = cache->capacity;
    kvp_t *old_cells = cache->cells;
    if (old_capacity >= INITIAL_CAPACITY) {
        cache->capacity = old_capacity * 2;
    } else {
        cache->capacity = INITIAL_CAPACITY * 2;
    }
    cache->cells = calloc(cache->capacity, sizeof(kvp_t));
    for (int i = 0; i < old_capacity; i++) {
        kvp_t *old_cell = &old_cells[i];
        if (!old_cell->value) {
            continue;
        }
        size_t index = cache_get_index(cache, &old_cell->key);
        cache->cells[index] = *old_cell;
    }
    free(old_cells);
}

void cache_remember(cache_t *cache, obj_list_t *args, object_t *value) {
    kvp_t *cell = cache_find_cell(cache, args);
    if (cell->value) {
        obj_unref(cell->value);
        cell->value = obj_ref(value);
        return;
    }
    if (++cache->count >= cache->capacity * LOAD_FACTOR) {
        cache_grow(cache);
        cell = cache_find_cell(cache, args);
    }
    obj_list_copy(&cell->key, args);
    cell->value = obj_ref(value);
}
