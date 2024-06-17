#include "iterator.h"
#include "json.h"
#include <stdio.h>

void do_indent(size_t indent) {
    printf("\n");
    for (size_t i = 0; i < indent; i++) {
        printf(" ");
    }
}

void beautify_array(Array *array, size_t indent, size_t *indent_ptr);

void beautify_object(Object *object, size_t indent, size_t *indent_ptr);

void beautify_attr(Attribute *attr, size_t indent, size_t *indent_ptr) {
    switch (attr->type) {
        case Array_T:
            beautify_array((Array*)attr->attr, indent, indent_ptr);
            break;
        case Object_T:
            beautify_object((Object*)attr->attr, indent, indent_ptr);
            break;
        default:
            print_attr(attr);
            break;
    }
}

void beautify_array(Array *array, size_t indent, size_t *indent_ptr) {
    Iterator it = iter_array(array);
    if (it.len == 0) {
        printf("[]");
    } else {
        printf("[");
    }
    for (Attribute *attr = next(&it); attr != NULL; attr = next(&it)) {
        do_indent(indent);
        beautify_attr(attr, indent + *indent_ptr, indent_ptr);
        if (it.position < it.len) {
            printf(",");
        } else {
            do_indent(indent - *indent_ptr);
            printf("]");
        }
    }
}

void beautify_json(Json *json, size_t indent, size_t *indent_ptr) {
    print(&json->key);
    printf(": ");
    beautify_attr(&json->value, indent + *indent_ptr, indent_ptr);
}

void beautify_object(Object *object, size_t indent, size_t *indent_ptr) {
    Iterator it = iter_object(object);
    if (it.len == 0) {
        printf("{}");
    } else {
        printf("{");
    }
    for (Json *pair = next(&it); pair != NULL; pair = next(&it)) {
        do_indent(indent);
        beautify_json(pair, indent, indent_ptr);
        if (it.position < it.len) {
            printf(",");
        } else {
            do_indent(indent - *indent_ptr);
            printf("}");
        }
    }
}

void beautify(Object *object, size_t indent) {
    beautify_object(object, indent, &indent);
    printf("\n");
}

