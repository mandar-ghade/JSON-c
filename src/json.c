#include "string.h"
#include "vec.h"
#include "iterator.h"
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

void print_attr(Attribute *attr);

void print_json(Json *json) {
    print(&json->key);
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
            print((String*)attr->attr);
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
            break;
        case Number:
            break;
        case Bool:
            free(attr->attr);
            break;
        default:
            break;
    }
}

void free_json(Json *json) {
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

Attribute index_from_attr(Attribute attr, size_t index);

Attribute get_from_attr(Attribute attr, char *key);

Iterator iter_from_attr(Attribute *attr);

Attribute null_attr() {
    Attribute attr = {
        .attr = NULL,
        .type = Null,
    };
    return attr;
}

Iterator iter_array(void *array); 

Iterator iter_object(void *object); 

Iterator iter_from_attr(Attribute *attr) {
    if (attr != NULL && attr->type == Array_T) {
        return iter_array((Array*)attr->attr);
    }
    if (attr != NULL && attr->type == Object_T) {
        return iter_object((Object*)attr->attr);
    }
    return empty_iter();
}

Attribute get(Object *object, char *key); 

Attribute index_array(Array *arr, size_t index); 

Attribute index_from_attr(Attribute attr, size_t index) {
    Attribute res = null_attr();
    if (attr.type == Array_T) {
        Attribute attr_res = index_array((Array*)attr.attr, index);
        res = attr_res;
    }
    return res;
}

Attribute get_from_attr(Attribute attr, char *key) {
    Attribute res = null_attr();
    if (attr.type == Object_T) {
        Attribute attr_res = get((Object*)attr.attr, key);
        res = attr_res;
    }
    return res;
}

Attribute *get_attr_from_vec(const Vec *vec, size_t index) {
    if (index >= vec->len) {
        return NULL;
    }
    return (Attribute*)get_value(vec, index);
}

Iterator iter_array(void *array) {
    Iterator it = iter_from_vec(&((Array*)array)->attrs);
    return it;
}

Attribute index_array(Array *arr, size_t index) {
    Attribute *attribute = get_attr_from_vec(&arr->attrs, index);
    if (attribute != NULL) {
        return *attribute;
    }
    return null_attr();
}

Array new_array() {
    Array array = {
        .attrs = new_vec(sizeof(Attribute)),
    };
    return array;
}

Array *new_heap_array() {
    Array *array = (Array*)malloc(sizeof(Array));
    array->attrs = new_vec(sizeof(Attribute));
    return array;
}

Iterator iter_object(void *object) {
    Iterator it = iter_from_vec(&((Object*)object)->pairs);
    return it;
}

Json *get_json_from_vec(const Vec *vec, char *key) {
    String key_str = new_string(key);
    Json *json = NULL;
    for (size_t i = 0; i < vec->len; i++) {
        Json *local_json = (Json*)get_value(vec, i);
        if (local_json != NULL && are_equal(&key_str, &local_json->key)) {
            json = local_json;
            break;
        }
    }
    reset_str(&key_str);
    return json;
}

Attribute get(Object *obj, char *key) {
    Json *json = get_json_from_vec(&obj->pairs, key);
    if (json != NULL) {
        return json->value;
    }
    return null_attr();
}

Object new_object() {
    Object object = {
        .pairs = new_vec(sizeof(Json)),
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
        .type = type,
    };
    return attribute;
}

Json create_json(String *key, void *value, Identifier type) {
    Attribute attr = new_attr(value, type);
    Json pair = new_json(key, &attr);
    return pair;
}

void example_json() {
    //char *test_pair = "{"base_key": {"key": null}}";
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
