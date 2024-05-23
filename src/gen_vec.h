#ifndef GEN_VEC_
#define GEN_VEC_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t len;
    size_t cap;
    size_t element_size;
    void* original_data;
    void* data;
} GenVec;

GenVec new_gen_vec(const size_t element_size); 

void free_gen_vec(GenVec *vec); 

GenVec* heap_allocated_gen_vec(const size_t element_size); 

void reallocate_gen_vec(GenVec* vec, size_t cap); 

void* get_value(GenVec* vec, size_t index); 

void set_value(GenVec* vec, size_t index, void* val); 

void push_to_gen_vec(GenVec* vec, void* val); 

GenVec* convert_to_generic_vec(void **data, const size_t element_size, const size_t start, const size_t end, bool is_gen_vec); 

void test_gen_vec(); 
#endif // GEN_VEC_
