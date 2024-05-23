#ifndef JSON_TOKENIZATION_
#define JSON_TOKENIZATION_
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    String_t,
    Int_t,
    Number_t,
    Comma,
    Colon,
    Lbracket,
    Rbracket,
    Lcurly,
    Rcurly,
    NullType,
    BoolTrue,
    BoolFalse,
} TokenType;


typedef struct {
    TokenType type;
    size_t len;
    size_t cap;
    char* value;
} Token;

typedef struct {
    Token** tokens;
    size_t len;
    size_t cap;
} TokenVec;


Token* new_token(); // for testing purposes. 

void push_to_token(Token *tk, char c);  // for testing purposes.

void print_token(Token *tk); 

void print_token_vec(TokenVec *vec); 

TokenVec* tokenize(char *json_str); 

void deallocate_tk(Token* tk); 

void deallocate_vec(TokenVec *vec); 

void interactive_tokenization_testing(); 

#endif // JSON_TOKENIZATION_ 
