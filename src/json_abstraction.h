#ifndef JSON_ABSTRACTION_
#define JSON_ABSTRACTION_
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    Int,
    Number,
    String,
    Bool,
    Null,
    Delimiter,
    JsonArray_T,
    JsonPair_T, // new, unused in json_abstraction
    Json_T,
} JsonType;

typedef struct {
    void* raw; // includes JsonArray and Json
    size_t raw_size;
    JsonType type;
} Val;

typedef struct {
    Val* value;  // only 1
} JsonVal; 

typedef struct {
    Val** array;
    size_t len;
    size_t cap;
} JsonArray; // literally a bs vector.

typedef struct {
    char* key;
    JsonVal* value; // only one
} JsonPair;

typedef struct {
    JsonPair** pairs;
    size_t len;
    size_t cap;
} Json;

void print_json_array(JsonArray *array); // debugging

void print_json_value(Val *value); // debugging 

void print_json_pair(JsonPair *json_pair); // debugging

void print_json(Json *json); 

JsonArray* new_json_array(); 

void push_to_json_array(JsonArray *array, Val *val); 

Json* new_json(); 

void push_to_json(Json *json, JsonPair *pair); 

JsonPair* create_pair(char *key, JsonVal* value); 

JsonVal* initialize_json_value(Val *value); 

Val* initialize_generic_value(void *value, JsonType type); 

JsonPair* new_json_pair(char *key, void *value, JsonType type); 

void deallocate_array(JsonArray *array); // rare 

void deallocate_val(Val *val);  // rare

void deallocate_pair(JsonPair *pair); //rare

void deallocate_json(Json *json); 

void index_json(); 

void json_initialization_example(); // for testing.

#endif // JSON_ABSTRACTION_
