#include "data/cons.h"
#include "data/func.h"
#include "data/object.h"
#include "data/primitives.h"
#include "data/type.h"

typedef struct {
    object_t base;
    object_t *car;
    object_t *cdr;
} cons_t;

static result_t cons_eval(object_t *obj, env_t *env, bool *dirty) {
    cons_t *cons = (cons_t *)obj;
    object_t *args = cons->cdr;

    result_t func_res = obj_eval(cons->car, env, dirty);
    if (result_is_error(&func_res)) {
        return func_res;
    }

    result_t result = obj_call_func(func_res.object, cons->cdr, env, dirty);
    obj_unref(func_res.object);

    return result;
}

static void cons_print(const object_t *obj, FILE *file) {
    fputc('(', file);
    if (obj_is_cons(obj)) {
        cons_t *cons = (cons_t *)obj;
        obj_print(cons->car, file);
        obj = cons->cdr;
    }
    while (obj_is_cons(obj)) {
        cons_t *cons = (cons_t *)obj;
        fputc(' ', file);
        obj_print(cons->car, file);
        obj = cons->cdr;
    }
    if (!obj_is_null(obj)) {
        fprintf(file, " . ");
        obj_print(obj, file);
    }
    fputc(')', file);
}

static hash_t cons_hash(const object_t *obj) {
    cons_t *cons = (cons_t *)obj;
    return hash_combine(obj_hash(cons->car), obj_hash(cons->cdr));
}

static bool cons_equals(const object_t *obj, const object_t *other) {
    if (!obj_is_cons(other)) {
        return false;
    }
    cons_t *cons_a = (cons_t *)obj;
    cons_t *cons_b = (cons_t *)other;

    if (!obj_equals(cons_a->car, cons_b->car)) {
        return false;
    }
    if (!obj_equals(cons_a->cdr, cons_b->cdr)) {
        return false;
    }
    return true;
}

static void cons_free(object_t *obj) {
    cons_t *cons = (cons_t *)obj;
    obj_unref(cons->car);
    obj_unref(cons->cdr);
    obj_free_default(obj);
}

const obj_type_t TYPE_CONS = (obj_type_t){
    .name = "CONS",
    .base = &TYPE_T,
    .size = sizeof(cons_t),
    .eval = cons_eval,
    .print = cons_print,
    .hash = cons_hash,
    .equals = cons_equals,
    .free = cons_free,
};

object_t *obj_cons(object_t *car, object_t *cdr) {
    bool is_mutable = obj_is_mutable(car) || obj_is_mutable(cdr);
    cons_t *cons = obj_alloc_default(&TYPE_CONS, is_mutable);
    cons->car = obj_ref(car);
    cons->cdr = obj_ref(cdr);
    return &cons->base;
}

object_t *obj_car(object_t *obj) {
    if (!obj_is_cons(obj)) {
        return NIL;
    }
    cons_t *cons = (cons_t *)obj;
    return obj_ref(cons->car);
}

object_t *obj_cdr(object_t *obj) {
    if (!obj_is_cons(obj)) {
        return NIL;
    }
    cons_t *cons = (cons_t *)obj;
    return obj_ref(cons->cdr);
}
