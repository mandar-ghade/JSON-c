#ifndef VEC_
#define VEC_

#include <stdlib.h>
#include <string.h>
#include "iterator.h"

typedef struct {
    void *data;
    size_t element_size;
    size_t len;
    size_t cap;
} Vec;

Vec new_vec(const size_t element_size); 

Iterator iter_from_vec(const Vec *vec); 

Vec *heap_allocated_vec(const size_t element_size); 

void *get_value(const Vec *vec, const size_t index); 

void push_to_vec(Vec *vec, const void *value); 

void free_vec(Vec *vec); 

#endif // VEC_
