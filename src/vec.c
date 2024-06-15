#include <stdlib.h>
#include <string.h>
#include "iterator.h"

typedef struct {
    void *data;
    size_t element_size;
    size_t len;
    size_t cap;
} Vec;

Vec new_vec(const size_t element_size) {
    Vec vec = {
        .data = NULL,
        .element_size = element_size,
        .len = 0,
        .cap = 0,
    };
    return vec;
}

Iterator iter_from_vec(const Vec *vec) {
    Iterator iterator = new_iter(vec->data, vec->element_size, vec->len);
    return iterator;
}

Vec *heap_allocated_vec(const size_t element_size) {
    Vec *vec = (Vec*)malloc(sizeof(Vec));
    vec->data = NULL;
    vec->element_size = element_size;
    vec->len = 0;
    vec->cap = 0;
    return vec;
}

void set_value(Vec *vec, const size_t index, const void *value) {
    memcpy(vec->data + (vec->element_size * index), value, vec->element_size);
}

void *get_value(const Vec *vec, const size_t index) {
    return (vec->data + (vec->element_size * index));
}

void push_to_vec(Vec *vec, const void *value) {
    if (vec->len == vec->cap) {
        vec->cap = (vec->len == 0) ? 1 : vec->len * 2;
        vec->data = (void*)realloc(vec->data, vec->element_size * vec->cap);
    }
    set_value(vec, vec->len, value);
    vec->len++;
}

void free_vec(Vec *vec) { // for heap allocated.
    if (vec->data) free(vec->data);
    free(vec);
}

