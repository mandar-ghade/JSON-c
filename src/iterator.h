#ifndef ITERATOR_
#define ITERATOR_
#include <string.h>
#include <stdbool.h>

typedef struct {
    void *collection;
    size_t position;
    size_t len;
    size_t element_size;
} Iterator;

Iterator new_iter(void *collection, size_t element_size, size_t len); 

bool has_next(Iterator *it); 

void *next(Iterator *it); 

void *peek(Iterator *it); 

void *peek_nth(Iterator *it, size_t n); 

#endif // ITERATOR_
