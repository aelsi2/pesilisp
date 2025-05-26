#include "object.h"
#include "type.h"
#include "primitives.h"

bool obj_is_mutable(const object_t *object) {
    return object->is_mutable;
}

const obj_type_t *obj_get_type(const object_t *object) {
    return object->type;
}

bool obj_of_type(const object_t *object, const obj_type_t *type) {
    return type_subtype_of(object->type, type);
}

result_t obj_eval(object_t *object, env_t *env) {
    return object->type->eval(object, env);
}

void obj_print(const object_t *object, FILE *file) {
    object->type->print(object, file);
}

hash_t obj_hash(const object_t *object) {
    return object->type->hash(object);
}

bool obj_equals(const object_t *object, const object_t *other) {
    return object->type->equals(object, other);
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
            object->type->free(object);
        }
    }
    return NIL;
}

