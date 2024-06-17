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

Token new_token(void *value, Token_T type) {
    Token token = {
        .value = value,
        .type = type
    };
    return token;
}

char char_token_types[6] = {'{', '[', ']', '}', ':', ','};

char *type_to_chars(const Token_T type) {
    switch (type) {
        case Lcurly: return "Lcurly";
        case Rcurly: return "Rcurly";
        case Lbracket: return "Lbracket";
        case Rbracket: return "Rbracket";
        case Colon: return "Colon";
        case Comma: return "Comma";
        case Number_T: return "Number_T";
        case BoolTrue: return "BoolTrue";
        case BoolFalse: return "BoolFalse";
        case String_Type: return "String_Type";
        case Null_T: return "Null_T";
        default: return "N/A";
    }
}

bool is_numeric(const char c) {
    int digit = c - '0';
    return (digit >= 0 && digit <= 9);
}

void print_token(const Token *token) {
    printf("Token(%s, ", type_to_chars(token->type));
    if (token->type == String_Type) {
        print(token->value);
    } else {
        print_raw(token->value);
    }
    printf(")");
}

void print_token_vec(const Vec *vec) {
    printf("TokenVec(");
    if (vec->len == 0) {
        printf(")\n");
        return;
    }
    print_token((Token*)get_value(vec, 0));
    for (size_t i = 1; i < vec->len; i++) {
        printf(",\n\t ");
        print_token((Token*)get_value(vec, i));
    }
    printf("\n\t)\n");
}

void free_token(Token *token) {
    reset_str(token->value);
    free(token->value);
}

void free_token_vec(Vec *vec) {
    if (!vec->data) {
        free(vec);
        return;
    }
    for (size_t i = 0; i < vec->len; i++) {
        free_token((Token*)get_value(vec, i));
    }
    free(vec->data);
}

Token_T char_to_type(char c) {
    switch (c) {
        case '{': return Lcurly;
        case '}': return Rcurly;
        case '[': return Lbracket;
        case ']': return Rbracket;
        case ':': return Colon;
        case ',': return Comma;
        default: return Null_T;
    }
}

bool is_single_char(char c) {
    for (size_t i = 0; i < strlen(char_token_types); i++) {
        if (char_token_types[i] == c) return true;
    }
    return false;
}

bool is_end_pair_delim(Token_T type) {
    return (type == Rcurly || type == Comma || type == Rbracket);
}

char *next_non_space(Iterator *it) {
    char *peeked_char = peek(it);
    for (size_t i = 1; peeked_char != NULL; i++) {
        if (*peeked_char != ' ' &&
            *peeked_char != 10 &&  // 10 == \n
            *peeked_char != 9) break; // 9 == \t
        char *buffer_char = peek_nth(it, i);
        if (buffer_char == NULL) break;
        peeked_char = buffer_char;
    }
    return peeked_char;
}

bool check_string_equality(Iterator *it, char start_char, char *string) {
    if (start_char != string[0]) return false;
    for (size_t i = 1; i < strlen(string); i++) { // 1 char offset
        char *peeked = peek_nth(it, i - 1);
        if (peeked == NULL || *peeked != string[i]) return false;
    }
    return true;
}

Token_T is_keyword(Token_T type) {
    return (type == BoolTrue || type == BoolFalse || type == Null_T);
}

char *keyword_to_str(Token_T type) {
    switch (type) {
        case BoolTrue: return "true";
        case BoolFalse: return "false";
        case Null_T: return "null";
        default: return "N/A";
    }
}

Token_T get_type(Iterator *it, char curr) {
    if (curr == '"') return String_Type;

    bool numeric = is_numeric(curr);
    bool next_is_numeric = false;
    if (peek(it) != NULL) next_is_numeric = is_numeric(*(char*)peek(it));
    if (numeric || (curr == '-' && next_is_numeric)) {
        return Number_T;
    }

    bool is_single_char_match = is_single_char(curr);
    Token_T matched_char_to_type = char_to_type(curr);
    if (is_single_char_match) return matched_char_to_type;

    bool is_true = check_string_equality(it, curr, "true");
    if (is_true) return BoolTrue;

    bool is_false = check_string_equality(it, curr, "false");
    if (is_false) return BoolFalse;

    bool is_null = check_string_equality(it, curr, "null");
    if (is_null) return Null_T;

    return String_Type;
}

bool is_string_terminating_char(char c) {
    return (c == ':' || c == ',' || c == '}' || c == ']');
}

void push_keyword_to_string(Iterator *it, String *string, Token_T type) {
    char *keyword_str = keyword_to_str(type);
    for (size_t i = 0; i < strlen(keyword_str); i++) {
        push(string, keyword_str[i]);
        if (i != strlen(keyword_str) - 1) next(it); // increments position.
    }
}

void concatenate_into_token_str(Iterator *it, char *ptr, String *token_str, Token_T token_type, bool *token_found) {
    size_t q_count = 0;
    size_t decimal_count = 0;
    size_t sign_count = 0;


    for (char *char_ptr = ptr; !*token_found && char_ptr != NULL; char_ptr = next(it)) {
        char character = *char_ptr;
        char *peeked_char = next_non_space(it);

        if (token_type == String_Type 
            && is_string_terminating_char(*peeked_char) 
            && q_count > 0 
            && character == '"' 
            && (++q_count % 2 == 0)) *token_found = true;
        else if (token_type == String_Type && character == '"') {
            q_count++;
        } else if (token_type == Number_T 
                 && !is_numeric(character)
                 && !(character == '.' && decimal_count == 0)
                 && !(character == '-' && sign_count == 0)) {
            *token_found = true;
        } else if (token_type == Number_T && is_end_pair_delim(char_to_type(*peeked_char))) {
            push(token_str, character);
            *token_found = true;
        }

        if (token_type == String_Type) {
            if (*token_found) break;
            if ((q_count > 1 && character == '"') || 
                (character != '"')) push(token_str, character);
        } else if (*token_found && token_type == Number_T) {
            break;
        } else if (!*token_found && token_type == Number_T) {
            push(token_str, character);
            if (character == '.') decimal_count++;
            if (character == '-') sign_count++;
        }
    }

}

Vec tokenize(String *str) {
    Vec vec = new_vec(sizeof(Token));
    Iterator it = iter_from_vec(&str->vec);
    for (char *ptr = next(&it); ptr != NULL; ptr = next(&it)){
        char c = *ptr;
        if (c == ' ') continue; // only for non-string

        bool token_found = false;
        Token_T token_type = get_type(&it, c);

        if (c != '"' && token_type == String_Type) continue;

        if (token_type != String_Type && token_type != Number_T) {
            token_found = true;
        }

        String *token_str = heap_allocated_string("");

        if (is_keyword(token_type)) {
            push_keyword_to_string(&it, token_str, token_type);
        } else if (token_type != String_Type && token_type != Number_T) {
            push(token_str, c); // single char
        } else {
            concatenate_into_token_str(&it, ptr, token_str, token_type, &token_found);
        }

        if (token_found) {
            Token matched_token = new_token(token_str, token_type);
            push_to_vec(&vec, &matched_token);
        }
        else {
            reset_str(token_str);
            free(token_str);
        }
    }
    return vec;
}

int example_tokenize() {
    String json_str = new_string("{\"key\" : \"value\" ,     \"key 2\":[5.000001, \"five\", -5, true, null]}");
    Vec token_vec = tokenize(&json_str);
    print_token_vec(&token_vec);
    free_token_vec(&token_vec);
    reset_str(&json_str);
    return 0;
}
