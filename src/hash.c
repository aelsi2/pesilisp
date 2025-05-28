#include "hash.h"
#include <string.h>

const hash_t hash_default = 14695981039346656037u;

hash_t hash(const void *bytes, size_t length) {
    hash_t hash = hash_default;
    for (size_t i = 0; i < length; i++) {
        hash ^= ((char*)bytes)[i];
        hash *= 1099511628211u;
    }
    return hash;
}

hash_t hash_str(const char *str) {
    return hash(str, strlen(str));
}

hash_t hash_int64(int64_t value) {
    return hash(&value, sizeof(int64_t));
}

hash_t hash_combine(hash_t a, hash_t b) {
    hash_t hashes[] = { a, b };
    return hash(hashes, sizeof(hashes));
}
