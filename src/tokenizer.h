#ifndef JSON_TOKENIZER_
#define JSON_TOKENIZER_

#include "iterator.h"
#include "string.h"
#include "vec.h"

typedef enum {
    Lcurly,
    Rcurly,
    Lbracket,
    Rbracket,
    Colon,
    Comma,
    Number_T,
    BoolTrue,
    BoolFalse,
    String_Type, // not to be confused with String_T in Json
    Null_T,
} Token_T;

typedef struct {
    String *value; // String pointer used instead.
    Token_T type;
} Token; // used as a pointer in Vec


void free_token_vec(Vec *vec); 

void free_token(Token *token); // occasionally used

void print_token_vec(const Vec *vec); 

void print_token(const Token *token); // debugging.

Vec tokenize(String *str); 

#endif // JSON_TOKENIZER_
