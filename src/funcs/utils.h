#pragma once

#include "data/environment.h"
#include "data/func.h"
#include "data/list.h"
#include "data/primitives.h"

#define read_args(list, args, env) obj_list_t list = obj_flatten(args)

#define free_args(list) obj_list_free(&list)

#define args_eval_all(list, env, dirty)                                        \
    do {                                                                       \
        for (int i = 0; i < list.count; i++) {                                 \
            arg_eval(list, i, env, dirty);                                     \
        }                                                                      \
    } while (0)

#define arg_eval(list, index, env, dirty)                                      \
    do {                                                                       \
        result_t arg_eval_res = obj_eval(list.array[index], env, dirty);       \
        if (result_is_error(&arg_eval_res)) {                                  \
            obj_list_free(&list);                                              \
            return arg_eval_res;                                               \
        }                                                                      \
        obj_unref(list.array[index]);                                          \
        list.array[index] = arg_eval_res.object;                               \
    } while (0)

#define ensure_args_at_least(list, min_count)                                  \
    do {                                                                       \
        if (list.count < min_count) {                                          \
            obj_list_free(&list);                                              \
            return result_error(NULL);                                         \
        }                                                                      \
    } while (0)

#define ensure_args_between(list, min_count, max_count)                        \
    do {                                                                       \
        if (list.count < min_count) {                                          \
            obj_list_free(&list);                                              \
            return result_error(NULL);                                         \
        }                                                                      \
        if (list.count > max_count) {                                          \
            obj_list_free(&list);                                              \
            return result_error(NULL);                                         \
        }                                                                      \
    } while (0)

#define ensure_args_exactly(list, exact_count)                                 \
    do {                                                                       \
        if (list.count != exact_count) {                                       \
            obj_list_free(&list);                                              \
            return result_error(NULL);                                         \
        }                                                                      \
    } while (0)

#define ensure_integer(list, index)                                            \
    do {                                                                       \
        if (!obj_is_int(list.array[index])) {                                  \
            obj_list_free(&list);                                              \
            return result_error(NULL);                                         \
        }                                                                      \
    } while (0)

#define env_register(env, name, func)                                          \
    do {                                                                       \
        object_t *func_obj = obj_make_native_func(func);                       \
        env_define(env, name, func_obj);                                       \
        obj_unref(func_obj);                                                   \
    } while (0)
