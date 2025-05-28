#pragma once

#include "data/environment.h"
#include "data/func.h"
#include "data/list.h"
#include "data/object.h"
#include "data/primitives.h"
#include <limits.h>

#define list_begin(list, args) obj_list_t list = obj_flatten(args)

#define list_end(list) obj_list_free(&list)

#define args_eval_all(list, env, recursion_limit, dirty)                       \
    do {                                                                       \
        for (int i = 0; i < list.count; i++) {                                 \
            arg_eval(list, i, env, recursion_limit, dirty);                    \
        }                                                                      \
    } while (0)

#define arg_eval(list, index, env, recursion_limit, dirty)                     \
    do {                                                                       \
        result_t arg_eval_res =                                                \
            obj_eval(list.array[index], env, recursion_limit, dirty);          \
        if (result_is_error(&arg_eval_res)) {                                  \
            obj_list_free(&list);                                              \
            return arg_eval_res;                                               \
        }                                                                      \
        obj_unref(list.array[index]);                                          \
        list.array[index] = arg_eval_res.object;                               \
    } while (0)

#define ensure_args_at_least(func, list, min_count)                            \
    do {                                                                       \
        if (list.count < min_count) {                                          \
            error_t *error =                                                   \
                error_wrong_arg_count(obj_func_name((object_t *)func),         \
                                      min_count, INT_MAX, list.count);         \
            obj_list_free(&list);                                              \
            return result_error(error);                                        \
        }                                                                      \
    } while (0)

#define ensure_args_at_most(func, list, max_count)                             \
    do {                                                                       \
        if (list.count > max_count) {                                          \
            error_t *error = error_wrong_arg_count(                            \
                obj_func_name((object_t *)func), 0, max_count, list.count);    \
            obj_list_free(&list);                                              \
            return result_error(error);                                        \
        }                                                                      \
    } while (0)

#define ensure_args_between(func, list, min_count, max_count)                  \
    do {                                                                       \
        if (list.count < min_count || list.count > max_count) {                \
            error_t *error =                                                   \
                error_wrong_arg_count(obj_func_name((object_t *)func),         \
                                      min_count, max_count, list.count);       \
            obj_list_free(&list);                                              \
            return result_error(error);                                        \
        }                                                                      \
    } while (0)

#define ensure_args_exactly(func, list, exact_count)                           \
    do {                                                                       \
        if (list.count != exact_count) {                                       \
            error_t *error =                                                   \
                error_wrong_arg_count(obj_func_name((object_t *)func),         \
                                      exact_count, exact_count, list.count);   \
            obj_list_free(&list);                                              \
            return result_error(error);                                        \
        }                                                                      \
    } while (0)

#define ensure_type(func, list, index, type)                                   \
    do {                                                                       \
        if (obj_of_type(list.array[index], (type))) {                          \
            break;                                                             \
        }                                                                      \
        const obj_type_t *actual = obj_get_type(list.array[index]);            \
        const char *func_name = obj_func_name((object_t *)func);               \
        error_t *error = error_wrong_arg_type(func_name, index, type, actual); \
        obj_list_free(&list);                                                  \
        return result_error(error);                                            \
    } while (0)

#define ensure_list(func, list, index)                                         \
    do {                                                                       \
        if (obj_is_list(list.array[index])) {                                  \
            break;                                                             \
        }                                                                      \
        const obj_type_t *actual = obj_get_type(list.array[index]);            \
        const char *func_name = obj_func_name((object_t *)func);               \
        error_t *error = error_arg_not_list(func_name, index, actual);         \
        obj_list_free(&list);                                                  \
        return result_error(error);                                            \
    } while (0)

#define env_register_func(env, name, func)                                     \
    do {                                                                       \
        object_t *func_obj = obj_make_native_func(name, func);                 \
        env_define(env, name, func_obj);                                       \
        obj_unref(func_obj);                                                   \
    } while (0)

error_t *error_wrong_arg_count(const char *name, int min, int max, int actual);

error_t *error_wrong_arg_type(const char *func_name, int arg_index,
                              const obj_type_t *expected,
                              const obj_type_t *actual);

error_t *error_arg_not_list(const char *func_name, int arg_index,
                            const obj_type_t *actual);
