#include "object.h"
#include "internal.h"
#include "primitives.h"

obj_type_t obj_type(const object_t *object) {
    return object->vtable->type;
}

result_t obj_eval(object_t *object, env_t *env) {
    return object->vtable->eval(object, env);
}

void obj_print(const object_t *object, FILE *file) {
    object->vtable->print(object, file);
}

hash_t obj_hash(const object_t *object) {
    return object->vtable->hash(object);
}

object_t *obj_ref(object_t *object) {
    if (object->ref_count != REFCOUNT_OFF) {
        object->ref_count++;
    }
    return object;
}

object_t *obj_unref(object_t *object) {
    if (object->ref_count != REFCOUNT_OFF) {
        if (!--object->ref_count) {
            object->vtable->free(object);
        }
    }
    return NIL;
}

