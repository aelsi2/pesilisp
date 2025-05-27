#pragma once

#include "data/environment.h"

void env_load_base(env_t *env);

void env_load_funcdef(env_t *env);

void env_load_arithmetic(env_t *env);

void env_load_boolean(env_t *env);

void env_load_lists(env_t *env);

void env_load_io(env_t *env);

void env_load_interactive(env_t *env);
