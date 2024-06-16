#ifndef JSON_
#define JSON_
#include "string.h"
#include "vec.h"
#include <stdbool.h>
typedef enum {
    Array_T,
    Object_T,
    String_T,
    Number,
    Bool,
    Null,
} Identifier;

typedef struct Attribute Attribute;

struct Attribute {
    void *attr;
    Identifier type;
    Attribute (*get)(void *attribute, char *key);
    Attribute (*index)(void *attribute, size_t index);
    Iterator (*iter)(void *array);
};

typedef struct {
    Vec attrs; // Vector of attributes.
    Attribute (*index)(void *array, size_t index);
    Iterator (*iter)(void *array);
} Array;

typedef struct {
    String key;
    Attribute value;
} Json;

typedef struct {
    Vec pairs; // Vector of Json structs.
    Attribute (*get)(void *object, char *key);
    Iterator (*iter)(void *object);
} Object;

void *index_object(Object *object);

void print_attr(Attribute *attr); 

void print_json(Json *json); // for debugging and iterating

void print_array(Array *array); // for debugging and iterating

void print_object(Object *object); 

void push_to_array(Array *array, Attribute *attr); 

Array new_array(); 

Array *new_heap_array();

void push_to_object(Object *object, Json *json); 

Object new_object(); 

Object *new_heap_object();

Attribute new_attr(void *attr, Identifier type); 

void free_object(Object *object);

Json new_json(String *key, Attribute *value); // only for debugging.

Json create_json(String *key, void *value, Identifier type); 

#endif // JSON_
