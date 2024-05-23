#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "json_abstraction.h"

void todo() {
    fprintf(stderr, "Not implemented yet");
    exit(1);
}

void print_json(Json *json);

void print_json_value(Val *value);

void print_json_array(JsonArray *array) {
    if (array->len == 0) {
        printf("[]");
        return;
    }
    printf("[");
    print_json_value(array->array[0]);
    for (size_t i = 1; i < array->len; i++) {
        printf(", ");
        print_json_value(array->array[i]);
    }
    printf("]");
}

void print_json_value(Val *value) {
    switch (value->type) {
        case Int:
            //printf("%d", *(int*)value->raw);
            printf("%s", (char*)value->raw);
            break;
        case Number:
            //printf("%f", *(float*)value->raw);
            printf("%s", (char*)value->raw);
            break;
        case String:
            //printf("\"%s\"", (char*)value->raw);
            printf("%s", (char*)value->raw);
            break;
        case Bool:
            printf((strcmp((char*)value->raw, "true")) ? "false" : "true");
            break;
        case Null:
            printf("null");
            break;
        case JsonArray_T:
            print_json_array((JsonArray*)value->raw);
            break;
        case Json_T:
            print_json((Json*)value->raw);
            break;
        default:
            printf("ERROR");
            break;
    }
}

void print_json_pair(JsonPair *json_pair) {
    printf("%s: ", json_pair->key);
    print_json_value(json_pair->value->value);
}

void print_json(Json *json) {
    if (json->len == 0) {
        printf("{}\n");
        return;
    }
    printf("{");
    print_json_pair(json->pairs[0]);
    for (size_t i = 1; i < json->len; i++) {
        printf(", ");
        print_json_pair(json->pairs[i]);
    }
    printf("}");
}

JsonArray* new_json_array() {
    JsonArray *array = malloc(sizeof(JsonArray));
    array->array = NULL;
    array->len = 0;
    array->cap = 0;
    return array;
}

void reallocate_json_array(JsonArray *array, size_t size) {
    array->cap = size;
    Val **new_array = (Val**)malloc(sizeof(Val*) * array->cap);
    for (size_t i = 0; i < array->len; i++) {
        new_array[i] = array->array[i];
    }
    free(array->array); // also deallocate after program execution.
    array->array = new_array;
}

void push_to_json_array(JsonArray *array, Val *val) {
    if (array->len == array->cap) {
        reallocate_json_array(array, array->len + 5);
    }
    array->array[array->len] = val; // must deallocate after program execution.
    array->len++;
}


Json* new_json() {
    Json *json = (Json*)malloc(sizeof(Json));
    json->pairs = NULL;
    json->len = 0;
    json->cap = 0;
    return json;
}

void reallocate_json(Json *json, size_t size) {
    json->cap = size;
    JsonPair **json_pairs = (JsonPair**)malloc(sizeof(JsonPair*) * json->cap);
    for (size_t i = 0; i < json->len; i++) {
        json_pairs[i] = json->pairs[i];
    }
    free(json->pairs); //deallocate json after program execution.
    json->pairs = json_pairs;
}

void push_to_json(Json *json, JsonPair *pair) {
    if (json->len == json->cap) {
        reallocate_json(json, json->len + 5);
    }
    json->pairs[json->len] = pair;
    json->len++;
}

JsonPair* create_pair(char *key, JsonVal* value) {
    JsonPair* pair = (JsonPair*)malloc(sizeof(JsonPair));
    pair->key = key;
    pair->value = value;
    return pair;
}

JsonVal* initialize_json_value(Val *value) {
    JsonVal* json_val = (JsonVal*)malloc(sizeof(JsonVal));
    json_val->value = value;
    return json_val;
}

Val* initialize_generic_value(void *value, JsonType type) {
    Val* val = (Val*)malloc(sizeof(Val));
    val->raw = value;
    val->type = type;
    return val;
}

JsonPair* new_json_pair(char *key, void *value, JsonType type) {
    Val *val = initialize_generic_value(value, type);
    JsonVal *json_val = initialize_json_value(val);
    JsonPair *json_pair = create_pair(key, json_val);
    return json_pair;
}

void deallocate_array(JsonArray *array) {
    for (size_t i = 0; i < array->len; i++) {
        deallocate_val(array->array[i]);
    }
    free(array);
}

void deallocate_val(Val *val) {
    if (val->type == JsonArray_T) {
        deallocate_array((JsonArray*)val->raw);
    }
    else if (val->type == Json_T) {
        deallocate_json((Json*)val->raw);
    }
    free(val);
}

void deallocate_pair(JsonPair *pair) {
    if (pair->value) { // JsonVal*
        if (pair->value->value) {
            deallocate_val(pair->value->value);
        }
        free(pair->value);
    }
    free(pair);
}

void deallocate_json(Json *json) {
    for (size_t i = 0; i < json->len; i++) {
        deallocate_pair(json->pairs[i]);
    }
    free(json->pairs);
    free(json);
}

void index_json() {
    todo();
}

void json_initialization_example() {
    Json *json = new_json();
    JsonArray *json_array = new_json_array();
    JsonArray *json_array_2 = new_json_array();
    push_to_json_array(json_array, initialize_generic_value("anorexia soup", String));
    push_to_json_array(json_array, initialize_generic_value("soy", String));
    push_to_json_array(json_array_2, initialize_generic_value("before", String));
    push_to_json_array(json_array_2, initialize_generic_value("nothing (zero food)", String));
    push_to_json_array(json_array_2, initialize_generic_value(json_array, JsonArray_T));
    push_to_json_array(json_array_2, initialize_generic_value("after", String));
    JsonPair *favorite_foods = new_json_pair("favorite foods", json_array_2, JsonArray_T);
    push_to_json(json, new_json_pair("name", "Mandar", String));
    int date_count = -1;
    push_to_json(json, new_json_pair("dates", &date_count, Int));
    push_to_json(json, new_json_pair("single", "true", Bool));
    push_to_json(json, new_json_pair("is_stupid", "true", Bool));
    push_to_json(json, favorite_foods);
    print_json(json);
    deallocate_json(json);
}
