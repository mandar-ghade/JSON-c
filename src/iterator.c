#include <stdbool.h>
#include <string.h>


typedef struct {
    void *collection;
    size_t position;
    size_t len;
    size_t element_size;
} Iterator;

Iterator new_iter(void *collection, size_t element_size, size_t len) {
    Iterator iterator = {
        .collection = collection,
        .position = 0,
        .len = len,
        .element_size = element_size
    };
    return iterator;
}

Iterator empty_iter() {
    Iterator iterator = {
        .collection = NULL,
        .position = 0,
        .len = 0,
        .element_size = 0
    };
    return iterator;
}

bool has_next(Iterator *it) {
    return it->position < it->len;
}

void *next(Iterator *it) {
    if (!has_next(it)) return NULL;
    return it->collection + (it->element_size * it->position++);
}

void *peek(Iterator *it) {
    if (!has_next(it)) return NULL;
    return it->collection + (it->element_size * it->position);
}

void *peek_nth(Iterator *it, size_t n) {
    size_t position = it->position + n;
    if (position >= it->len) return NULL;
    return it->collection + (it->element_size * position);
}
