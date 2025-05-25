#include "primitives.h"
#include "data/object.h"
#include "type.h"

typedef struct {
    object_t base;
    intval_t value;
} integer_t;

static void nil_print(const object_t *object, FILE *file) {
    fprintf(file, "NIL");
}

static void t_print(const object_t *object, FILE *file) {
    fprintf(file, "T");
}

static void integer_print(const object_t *object, FILE *file) {
    fprintf(file, "%lld", ((integer_t *)object)->value);
}

static hash_t integer_hash(const object_t *object) {
    return hash_int64(((integer_t *)object)->value);
}

static bool integer_equals(const object_t *object, const object_t *other) {
    if (!obj_is_int(other)) {
        return false;
    }
    return obj_get_int(object) == obj_get_int(other);
}

const obj_type_t TYPE_T = (obj_type_t) {
    .name = "T",
    .base = NULL,
    .size = sizeof(object_t),
    .is_primitive = true,
    .eval = obj_eval_self,
    .print = t_print,
    .hash = obj_hash_default,
    .equals = obj_equals_default,
    .free = obj_free_noop,
};

object_t *const T = &(object_t){
    .type = &TYPE_T,
    .ref_count = REFCOUNT_OFF,
};

const obj_type_t TYPE_NULL = (obj_type_t) {
    .name = "NULL",
    .base = &TYPE_T,
    .size = sizeof(object_t),
    .is_primitive = true,
    .eval = obj_eval_self,
    .print = nil_print,
    .hash = obj_hash_default,
    .equals = obj_equals_default,
    .free = obj_free_noop,
};

object_t *const NIL = &(object_t){
    .type = &TYPE_NULL,
    .ref_count = REFCOUNT_OFF,
};

const obj_type_t TYPE_INT = (obj_type_t){
    .name = "INTEGER",
    .base = &TYPE_T,
    .size = sizeof(integer_t),
    .is_primitive = true,
    .eval = obj_eval_self,
    .print = integer_print,
    .hash = integer_hash,
    .equals = integer_equals,
    .free = obj_free_default,
};

object_t *obj_make_int(intval_t value) {
    integer_t *result = obj_alloc_default(&TYPE_INT);
    result->value = value;
    return (object_t *)result;
}

intval_t obj_get_int(const object_t *object) {
    if (!obj_is_int(object)) {
        return 0;
    }
    return ((integer_t *)object)->value;
}
