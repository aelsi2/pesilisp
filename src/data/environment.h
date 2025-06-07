#pragma once

#include <stdbool.h>

typedef struct object_s object_t;

// Defines an environment - a set of variable bindings.
typedef struct env_s env_t;

// Creates a new environment.
env_t *env_new(env_t *parent);

// Checks if a variable is defined in the environment or one of its ancestors.
bool env_is_defined(const env_t *env, const char *name);

// Defines a variable in the environment and sets it to the specified value.
void env_define(env_t *env, const char *name, object_t *value);

// Gets a variable from the environment or one of its ancestors
// depending on where it was defined.
// Returns NIL if the variable is undefined.
object_t *env_get(const env_t *env, const char *name);

// Makes a full copy of the environment.
env_t *env_copy(const env_t *env);

// Frees the environment.
void env_free(env_t *env);

