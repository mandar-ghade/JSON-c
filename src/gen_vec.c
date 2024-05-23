#include <stdbool.h>
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

GenVec new_gen_vec(const size_t element_size) {
    GenVec vec = {
        .len = 0,
        .cap = 0,
        .element_size = element_size,
        .data = NULL,
    };
    return vec;
}

void free_gen_vec(GenVec *vec) {
    if (vec->data) free(vec->data);
    //if (vec->original_data) free(vec->original_data);
    free(vec);
}

GenVec* heap_allocated_gen_vec(const size_t element_size) {
    GenVec *vec = (GenVec*)malloc(sizeof(GenVec));
    vec->len = 0;
    vec->cap = 0;
    vec->element_size = element_size;
    vec->data = NULL;
    vec->original_data = NULL;
    return vec;
}

void push_to_gen_vec(GenVec* vec, void* val);

void* get_value(GenVec* vec, size_t index);

GenVec* convert_to_generic_vec(void **data, const size_t element_size, const size_t start, const size_t end, bool is_gen_vec) { // [start, end]
    GenVec *vec = heap_allocated_gen_vec(element_size);
    if (!is_gen_vec) vec->original_data = data;
    for (size_t i = start; i < end + 1; i++) {
        push_to_gen_vec(vec, (is_gen_vec) ? (data + (i * element_size)) : &data[i]);
    }
    return vec;
}

void reallocate_gen_vec(GenVec* vec, size_t cap) {
    vec->cap = cap;
    void *data = malloc(vec->element_size * vec->cap);
    memcpy(data, vec->data, vec->len * vec->element_size);
    free(vec->data);
    vec->data = data;
}

void* get_value(GenVec* vec, size_t index) {
    return vec->data + (index * vec->element_size);
}

void set_value(GenVec* vec, size_t index, void* val) {
    memcpy(vec->data + (index * vec->element_size), val, vec->element_size);
}

void push_to_gen_vec(GenVec* vec, void* val) {
    if (vec->len == vec->cap) {
        reallocate_gen_vec(vec, vec->len + 5);
    }
    set_value(vec, vec->len, val);
    vec->len++;
}

void print_int_vec(GenVec* vec) {
    if (vec->len == 0) {
        printf("[]\n");
        return;
    }
    printf("[");
    printf("%d", *(int*)get_value(vec, 0));
    for (size_t i = 1; i < vec->len; i++) {
        printf(", %d", *(int*)get_value(vec, i));
    }
    printf("]\n");
}

void test_gen_vec() {
    int data_input[] = {1, 2, 3, 4, 5};
    GenVec vec = new_gen_vec(sizeof(int));
    for (size_t i = 0; i < 5; i++) {
        push_to_gen_vec(&vec, &data_input[i]);
    }
    print_int_vec(&vec);
    free(vec.data);
}
