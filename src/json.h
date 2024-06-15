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

typedef struct {
    void *attr;
    Identifier type;
} Attribute;

typedef struct {
    Vec attrs; // Vector of attributes.
} Array;

typedef struct {
    String key;
    Attribute value;
} Json;

typedef struct {
    Vec pairs; // Vector of Json structs.
} Object;

void *index_object(Object *object);

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
