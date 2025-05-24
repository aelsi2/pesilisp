#include "type.h"
#include "data/object.h"
#include <stdlib.h>

bool type_subtype_of(const obj_type_t *type, const obj_type_t *super) {
    while (type != NULL) {
        if (type == super) {
            return true;
        }
        type = type->base;
    }
    return false;
}

void *obj_alloc_default(const obj_type_t *const type) {
    object_t *object = calloc(1, type->size);
    object->type = type;
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
