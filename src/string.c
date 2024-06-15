#include "vec.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    Vec vec;
} String;

void print(String *str) {
    if (str->vec.len == 0) {
        putchar('"');
        putchar('"');
        return;
    }
    putchar('"');
    putchar(*(char*)get_value(&str->vec, 0));
    for (size_t i = 1; i < str->vec.len; i++) {
        putchar(*(char*)get_value(&str->vec, i));
    }
    putchar('"');
}

void print_raw(String *str) {
    if (str->vec.len == 0) {
        return;
    }
    putchar(*(char*)get_value(&str->vec, 0));
    for (size_t i = 1; i < str->vec.len; i++) {
        putchar(*(char*)get_value(&str->vec, i));
    }
}

void push_chars_to_string(String *string, char *str, size_t str_len) {
    for (size_t i = 0; i < str_len; i++) {
        push_to_vec(&string->vec, &str[i]);
    }
}

String empty_string() {
    String string = {
        .vec = new_vec(sizeof(char))
    };
    return string;
}

String new_string(char *str) {
    String string = {
        .vec = new_vec(sizeof(char))
    };
    push_chars_to_string(&string, str, strlen(str));
    return string;
}

String *heap_allocated_string(char *str) {
    String *string = (String*)malloc(sizeof(String));
    string->vec = new_vec(sizeof(char));
    push_chars_to_string(string, str, strlen(str));
    return string;
}

void push(String *string, char c) {
    push_to_vec(&string->vec, &c);
}

void reset_str(String *str) {
    free(str->vec.data);
    str->vec.len = 0;
    str->vec.cap = 0;
    str->vec.data = NULL;
}

char *string_to_chars(String *str) {
    char *old_str = (char*)malloc(str->vec.element_size * str->vec.len);
    for (size_t i = 0; i < str->vec.len; i++) {
        old_str[i] = *(char*)get_value(&str->vec, i);
    }
    return old_str;
}

void pop(String *str) {
    size_t str_len = str->vec.len - 1;
    char *old_str = string_to_chars(str);
    reset_str(str);
    push_chars_to_string(str, old_str, str_len);
    free(old_str);
}

bool are_equal(String *str1, String *str2) {
    return (!memcmp(str1->vec.data, str2->vec.data, sizeof(str1->vec.data)));
}

void example() {
    char *name = "John Doe";
    String name_str = new_string(name);
    String name_str_2 = new_string("John Doe");
    bool is_equal = are_equal(&name_str, &name_str_2);
    printf("Are equal: %s\n", is_equal ? "true" : "false");
    print(&name_str);
    print(&name_str_2);
    free(name_str.vec.data);
    free(name_str_2.vec.data);
}

