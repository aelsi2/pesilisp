#include "type.h"
#include "data/object.h"
#include <stdlib.h>

void *obj_alloc_default(const obj_type_t *const type, bool is_mutable) {
    object_t *object = calloc(1, type->size);
    object->type = type;
    object->is_mutable = is_mutable;
    object->ref_count = 1;
    return object;
}

void obj_free_default(object_t *object) {
    free(object);
}

result_t obj_eval_self(object_t *object, env_t *env, int recursion_limit, bool *dirty) {
    return result_success(obj_ref(object));
}

void obj_print_default(const object_t *object, FILE *file) {
    fprintf(file, "#<%s %p>", object->type->name, object);
}

hash_t obj_hash_default(const object_t *object) {
    return hash_int64((int64_t)object);
}

void obj_free_noop(object_t *object) {
}

bool obj_equals_default(const object_t *object, const object_t *other) {
    return object == other;
}
