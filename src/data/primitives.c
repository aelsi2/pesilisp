#include "primitives.h"
#include "internal.h"

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

object_t *const T = &(object_t){
    .vtable =
        &(obj_vtable_t){
            .type = TYPE_T,
            .eval = obj_eval_self,
            .print = t_print,
            .free = obj_free_noop,
        },
    .ref_count = REFCOUNT_OFF,
};

object_t *const NIL = &(object_t){
    .vtable =
        &(obj_vtable_t){
            .type = TYPE_NIL,
            .eval = obj_eval_self,
            .print = nil_print,
            .free = obj_free_noop,
        },
    .ref_count = REFCOUNT_OFF,
};

static obj_vtable_t *int_vtable = &(obj_vtable_t){
    .type = TYPE_INT,
    .eval = obj_eval_self,
    .print = integer_print,
    .free = obj_free_default,
};

object_t *obj_make_int(intval_t value) {
    integer_t *result = obj_alloc_default(int_vtable, sizeof(integer_t));
    result->value = value;
    return (object_t*)result;
}

intval_t obj_get_int(object_t *object) {
    if (!obj_is_int(object)) {
        return 0;
    }
    return ((integer_t*)object)->value;
}
