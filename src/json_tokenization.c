#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "json_tokenization.h"

char* type_to_enum_str(TokenType type) {
    switch (type) {
        case String_t:
            return "String_t";
        case Int_t:
            return "Int_t";
        case Number_t:
            return "Number_t";
        case Comma:
            return "Comma";
        case Colon:
            return "Colon";
        case Lbracket:
            return "Lbracket";
        case Rbracket:
            return "Rbracket";
        case Lcurly:
            return "Lcurly";
        case Rcurly:
            return "Rcurly";
        case NullType:
            return "NullType";
        case BoolTrue:
            return "BoolTrue";
        case BoolFalse:
            return "BoolFalse";
        default:
            return "ERROR";
    }
}

void print_token(Token *tk) {
    printf("Token(%s, %s)", type_to_enum_str(tk->type), tk->value);
}

void print_token_vec(TokenVec *vec) {
    if (vec->len == 0) {
        printf("TokenVec()\n");
    }
    printf("TokenVec(");
    print_token(vec->tokens[0]);
    for (size_t i = 1; i < vec->len; i++) {
        printf(", \n\t ");
        print_token(vec->tokens[i]);
    }
    printf(")\n");
}

void reallocate_vec(TokenVec *vec, size_t size) {
    vec->cap = size;
    Token** resized_array = (Token**)malloc(sizeof(Token*) * vec->cap);
    for (size_t i = 0; i < vec->len; i++) {
        resized_array[i] = vec->tokens[i];
    }
    free(vec->tokens);
    vec->tokens = resized_array;
}

void push_to_vec(TokenVec *vec, Token *tk) {
    if (vec->len == vec->cap) {
        reallocate_vec(vec, vec->len + 5);
    }
    vec->tokens[vec->len] = tk;
    vec->len++;
}

TokenVec* new_vec() {
    TokenVec* token_vec = (TokenVec*)malloc(sizeof(TokenVec)); // freed later
    token_vec->tokens = NULL;
    token_vec->len = 0;
    token_vec->cap = 0;
    return token_vec;
}

bool is_digit(char *c) {
    int digit = *c - '0';
    return (digit >= 0 && digit <= 9);
}

void set_token_type(Token *tk, char c) {
    if (tk->type == Int_t && !is_digit(&c) && c == '.') {
        tk->type = Number_t;
    }
    else if (tk->type == String_t && tk->value && 
             tk->value[0] == '"' && is_digit(&c)) { // so it doesnt interpret string as an int if it has numbers
        return;
    }
    else if (!tk->type && (c == '"' || c == '\'')) {
        tk->type = String_t;
    }
    else if (!tk->type && c == '{') {
        tk->type = Lcurly;
    }
    else if (!tk->type && c == '}') {
        tk->type = Rcurly;
    }
    else if (!tk->type && c == '[') {
        tk->type = Lbracket;
    }
    else if (!tk->type && c == ']') {
        tk->type = Rbracket;
    }
    else if (!tk->type && c == ',') {
        tk->type = Comma;
    }
    else if (!tk->type && c == ':') {
        tk->type = Colon;
    }
    else if (!tk->type && !strcmp(tk->value, "true")) {
        tk->type = BoolTrue;
    }
    else if (!tk->type && !strcmp(tk->value, "false")) {
        tk->type = BoolFalse;
    }
    else if (!tk->type && !strcmp(tk->value, "null")) {
        tk->type = NullType;
    }
    else if (!tk->type && is_digit(&c)) {
        tk->type = Int_t;
    }
}

bool check_break_char(char c) {
    return (c == ',' || c == ':' || c == ']' || c == '}');
}

bool is_opening_token(TokenType type) {
    return (type == Lcurly || type == Lbracket);
}

Token* new_token() {
    Token* token = malloc(sizeof(Token));
    token->value = NULL;
    token->len = 0;
    token->cap = 0;
    return token;
}

void push_to_token(Token *tk, char c) { // reallocates every time its pushed. 
    size_t cap = tk->len + 1;
    char* new_token_array = (char*)malloc(sizeof(char) * cap);
    for (size_t i = 0; i < tk->len; i++) {
        new_token_array[i] = tk->value[i];
    }
    if (tk->value) free(tk->value); // so it only frees if allocated.
    tk->value = new_token_array;
    tk->value[tk->len] = c;
    tk->len++;
}

bool is_invalid_space(Token *tk, char c) {
    return (((!tk->value && tk->type == String_t) || // if char behind lparen 
             !(tk->value && tk->value[0] == '"' 
               && tk->value[tk->len-1] != '"' && !tk->type)  // if char after rparen
            ) && c == ' '); // if char is space
}

TokenVec* tokenize(char *json_str) {
    size_t len = strlen(json_str);
    TokenVec* vec = new_vec();
    Token* prev = new_token();
    for (size_t i = 0; i < len; i++) {
        if (is_invalid_space(prev, json_str[i])) continue;
        if (check_break_char(json_str[i])) {
            if (!prev->value) deallocate_tk(prev);
            if (prev->value) push_to_vec(vec, prev);
            prev = new_token();
        }
        push_to_token(prev, json_str[i]);
        set_token_type(prev, json_str[i]);
        if (check_break_char(json_str[i])) {
            push_to_vec(vec, prev);
            prev = new_token();
        }
        else if (prev->type && is_opening_token(prev->type)) {
            push_to_vec(vec, prev);
            prev = new_token();
        }
    }
    if (prev->value) push_to_vec(vec, prev);
    else if (prev && !prev->value) deallocate_tk(prev);
    return vec;
}

void deallocate_tk(Token* tk) {
    if (tk->value) free(tk->value);
    free(tk);
}

void deallocate_vec(TokenVec *vec) {
    for (size_t i = 0; i < vec->len; i++) {
        deallocate_tk(vec->tokens[i]);
    }
    if (vec->tokens) free(vec->tokens);
    free(vec);
}

void interactive_tokenization_testing() {
    char json_str[500];
    printf("Enter json str: \n");
    scanf("%499[^\n]", json_str);
    TokenVec* token_vec = tokenize(json_str);
    print_token_vec(token_vec);
    deallocate_vec(token_vec);
}

