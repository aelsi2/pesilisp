#pragma once

#include <stddef.h>
#include <stdint.h>

typedef unsigned long long hash_t;

hash_t hash(const void *bytes, size_t length);
hash_t hash_str(const char *str);
hash_t hash_int64(int64_t value);
hash_t hash_combine(hash_t a, hash_t b);
