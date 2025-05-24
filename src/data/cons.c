#include "data/cons.h"
#include "data/internal.h"
#include "data/primitives.h"

typedef struct {
    object_t base;
    object_t *car;
    object_t *cdr;
} cons_t;

static result_t cons_eval(object_t *obj, env_t *env) {
    cons_t *cons = (cons_t*)obj;
    return obj_eval_self(obj, env);
}

static void cons_print(const object_t *obj, FILE *file) {
    fputc('(', file);
    if (obj_is_cons(obj)) {
        cons_t *cons = (cons_t*)obj;
        obj_print(cons->car, file);
        obj = cons->cdr;
    }
    while (obj_is_cons(obj)) {
        cons_t *cons = (cons_t*)obj;
        fputc(' ', file);
        obj_print(cons->car, file);
        obj = cons->cdr;
    }
    if (obj_get_bool(obj)) {
        fprintf(file, " . ");
        obj_print(obj, file);
    }
    fputc(')', file);
}

static void cons_free(object_t *obj) {
    cons_t *cons = (cons_t*)obj;
    obj_unref(cons->car);
    obj_unref(cons->cdr);
    obj_free_default(obj);
}

static obj_vtable_t *cons_vtable = &(obj_vtable_t){
    .type = TYPE_CONS,
    .eval = cons_eval,
    .print = cons_print,
    .free = cons_free,
};

object_t *obj_cons(object_t *car, object_t *cdr) {
    cons_t *cons = obj_alloc_default(cons_vtable, sizeof(cons_t));
    cons->car = obj_ref(car);
    cons->cdr = obj_ref(cdr);
    return &cons->base;
}

object_t *obj_car(object_t *obj) {
    if (!obj_is_cons(obj)) {
        return NIL;
    }
    cons_t *cons = (cons_t*)obj;
    return cons->car;
}

object_t *obj_cdr(object_t *obj) {
    if (!obj_is_cons(obj)) {
        return NIL;
    }
    cons_t *cons = (cons_t*)obj;
    return cons->cdr;
}
