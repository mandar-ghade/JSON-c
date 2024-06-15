#ifndef STRING_
#define STRING_
#include "vec.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    Vec vec;
} String;

void print(String *str); 

void print_raw(String *str);

String empty_string(); 

String new_string(char *str); 

String *heap_allocated_string(char *str);

void push(String *string, char c); 

void pop(String *str); 

bool are_equal(String *str1, String *str2);

void reset_str(String *str); 

#endif // STRING_
