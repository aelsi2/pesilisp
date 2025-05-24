#include "internal.h"
#include <stdlib.h>

void *obj_alloc_default(obj_vtable_t *vtable, size_t size) {
    object_t *object = calloc(1, size);
    object->vtable = vtable;
    object->ref_count = 1;
    return object;
}

void obj_free_default(object_t *object) {
    free(object);
}

result_t obj_eval_self(object_t *object, env_t *env) {
    return result_success(obj_ref(object));
}

hash_t obj_hash_default(const object_t *object) {
    return hash_int64((int64_t)object);
}

void obj_free_noop(object_t *object) {
}
