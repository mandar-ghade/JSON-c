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

void print_attr(Attribute *attr);

void print_json(Json *json) {
    print_raw(&json->key);
    printf(": ");
    print_attr(&json->value);
}

void print_array(Array *array) {
    if (array->attrs.len == 0) {
        printf("[]");
        return;
    }
    printf("[");
    print_attr((Attribute*)get_value(&array->attrs, 0));
    for (size_t i = 1; i < array->attrs.len; i++) {
        printf(", ");
        print_attr((Attribute*)get_value(&array->attrs, i));
    }
    printf("]");
}

void print_object(Object *object) {
    if (object->pairs.len == 0) {
        printf("{}");
        return;
    }
    printf("{");
    print_json((Json*)get_value(&object->pairs, 0));
    for (size_t i = 1; i < object->pairs.len; i++) {
        printf(", ");
        print_json((Json*)get_value(&object->pairs, i));
    }
    printf("}");
}

void print_attr(Attribute *attr) {
    switch (attr->type) {
        case Array_T:
            print_array((Array*)attr->attr);
            break;
        case Object_T:
            print_object((Object*)attr->attr);
            break;
        case String_T:
            print_raw((String*)attr->attr);
            break;
        case Number:
            print_raw((String*)attr->attr);
            break;
        case Bool:
            printf("%s", (*(bool*)attr->attr == true) ? "true" : "false");
            break;
        default:
            if (!attr->attr) printf("null");
            break;
    }
}

void free_attr(Attribute *attr);

void free_object(Object *object);

void free_array(Array *array) {
    for (size_t i = 0; i < array->attrs.len; i++) {
        free_attr((Attribute*)get_value(&array->attrs, i));
    }
    free(array->attrs.data);
    free(array);
}

void free_attr(Attribute *attr) {
    switch (attr->type) {
        case Array_T:
            free_array((Array*)attr->attr);
            break;
        case Object_T:
            free_object((Object*)attr->attr);
            break;
        case String_T:
//             free(((String*)attr->attr)->vec.data); // frees string data.
            break;
        case Number:
//             free(((String*)attr->attr)->vec.data); // also frees string data.
            break;
        case Bool:
            free(attr->attr);
            break;
        default:
            break;
    }
}

void free_json(Json *json) {
//     free(json->key.vec.data); // frees string data.
    free_attr(&json->value);
}

void free_object(Object *object) {
    for (size_t i = 0; i < object->pairs.len; i++) {
        free_json((Json*)get_value(&object->pairs, i));
    }
    free(object->pairs.data);
    free(object);
}

void push_to_array(Array *array, Attribute *attr) {
    push_to_vec(&array->attrs, attr);
}

bool get_if_keys_are_equal(Json *j, Json *k) {
    return are_equal(&j->key, &k->key);
}

bool get_if_key_exists(Object *object, Json *json) {
    for (size_t i = 0; i < object->pairs.len; i++) {
        Json *curr = get_value(&object->pairs, i);
        if (!get_if_keys_are_equal(curr, json)) return false;
    }
    return true;
}

void push_to_object(Object *object, Json *json) {
    if (object->pairs.len != 0 && get_if_key_exists(object, json)) return; // or switch the value.
    push_to_vec(&object->pairs, json);
}

Array new_array() {
    Array array = {
        .attrs = new_vec(sizeof(Attribute))
    };
    return array;
}

Array *new_heap_array() {
    Array *array = (Array*)malloc(sizeof(Array));
    array->attrs = new_vec(sizeof(Attribute));
    return array;
}

Object new_object() {
    Object object = {
        .pairs = new_vec(sizeof(Json))
    };
    return object;
}

Object *new_heap_object() {
    Object *object = (Object*)malloc(sizeof(Object));
    object->pairs = new_vec(sizeof(Json));
    return object;
}

Json new_json(String *key, Attribute *value) {
    Json json = {
        .key = *key,
        .value = *value
    };
    return json;
}

Attribute new_attr(void *attr, Identifier type) {
    Attribute attribute = {
        .attr = attr,
        .type = type
    };
    return attribute;
}

Json create_json(String *key, void *value, Identifier type) {
    Attribute attr = new_attr(value, type);
    Json pair = new_json(key, &attr);
    return pair;
}

void example_json() {
    //char *test_pair = "{"base_key": {"key": "value"}}";
    String key = new_string("key");
    Json pair = create_json(&key, NULL, Null);
    Object object = new_object();

    push_to_object(&object, &pair);

    String base_key = new_string("base_key");
    Json base_paired = create_json(&base_key, &object, Object_T);

    Object base = new_object();
    push_to_object(&base, &base_paired);

    print_object(&base);
    printf("\n");
    free_object(&base);
}
