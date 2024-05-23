#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "gen_vec.h"
#include "json_abstraction.h"
#include "json_tokenization.h"


Token* get_token_value(GenVec* vec, size_t index) {
    return *(Token**)get_value(vec, index);
}

void deallocate_generic_vec_and_tokens(GenVec *vec) { // last step
    free_gen_vec(vec);
}

void print_generic_vec_with_tokens(GenVec *vec); // debugging.

JsonType to_json_type(TokenType type) {
    switch (type) {
        case (String_t): return String;
        case (Int_t): return Int;
        case (Number_t): return Number;
        case (Colon): return JsonPair_T;
        case (Comma): return Delimiter;
        case (Lbracket): return JsonArray_T;
        case (Rbracket): return JsonArray_T;
        case (Lcurly): return Json_T; 
        case (Rcurly): return Json_T;
        case (NullType): return Null;
        case (BoolTrue): return Bool;
        case (BoolFalse): return Bool;
    }
}

char* json_type_to_str(JsonType type) {
    switch (type) {
        case (String): return "String";
        case (Int): return "Int";
        case (Number): return "Number";
        case (JsonPair_T): return "JsonPair_T";
        case (Delimiter): return "Delimiter";
        case (JsonArray_T): return "JsonArray_T";
        case (Json_T): return "Json_T";
        case (Null): return "NullType";
        case (Bool): return "Bool";
        default: return "None";
    }
}


const Val* parse_generic_value(const Token* value) {
    Val* val = initialize_generic_value(value->value, to_json_type(value->type));
    return val;
}

Json* parse_json(GenVec* token_vec, const size_t index_offset);

JsonArray* parse_json_array(GenVec* token_vec, const size_t offset) {
    JsonArray *json_array = new_json_array();
    void *upper_bound = NULL;
    const size_t max = token_vec->len;
    void *curr_value;
    JsonType curr_type;
    for (size_t i = offset; i < max; i++) {
        Token *tk = get_token_value(token_vec, i);
        if (upper_bound && i <= *(int*)upper_bound) continue;
        switch (tk->type) {
            case Comma:
                if (curr_value) {
                    push_to_json_array(json_array, initialize_generic_value(curr_value, curr_type));
                    //if (curr_type == JsonArray_T) {
                    //    free((JsonArray*)curr_value);
                    //}
                    //else if (curr_type == Json_T) {
                    //    free((Json*)curr_value);
                    //}
                    curr_value = NULL;
                    curr_type = Delimiter;
                }
                break;
            case Lcurly:
                curr_type = Json_T;
                GenVec *insertable_vec = convert_to_generic_vec(token_vec->original_data, token_vec->element_size, 0, max-1, false);
                curr_value = parse_json(insertable_vec, i + 1);
                size_t new_index = insertable_vec->len - 1;
                upper_bound = &new_index;
                deallocate_generic_vec_and_tokens(insertable_vec);
                break;
            case Rcurly:
                break;
            case Lbracket:
                if (i != offset) {
                    curr_type = JsonArray_T;
                    GenVec *insertable_vec_ = convert_to_generic_vec(token_vec->original_data, token_vec->element_size, 0, max-1, false);
                    curr_value = parse_json_array(insertable_vec_, i);
                    size_t new_i = insertable_vec_->len - 1;
                    upper_bound = &new_i;
                    deallocate_generic_vec_and_tokens(insertable_vec_);
                }
                break;
            case Rbracket:
                if (curr_value) {
                    push_to_json_array(json_array, initialize_generic_value(curr_value, curr_type));
                    curr_value = NULL;
                    curr_type = Delimiter;
                }
                break;
            default:
                curr_value = tk->value;
                curr_type = to_json_type(tk->type);
                break;
        }
        if (tk->type == Lbracket && i != 0) continue;
        token_vec->len = i + 1;
        if (tk->type == Rbracket) break;
    }
    return json_array;
}

Json* parse_json(GenVec* token_vec, const size_t index_offset) {
    Json *json = new_json();
    Token* key = NULL;
    void* value = NULL;
    void* upper_bound = NULL;
    size_t start = index_offset;
    size_t max = token_vec->len;
    JsonType value_type;
    for (size_t i = index_offset; i < max; i++) {
        Token* tk = get_token_value(token_vec, i);
        if (upper_bound && i <= *(size_t*)upper_bound) continue;
        switch (tk->type) {
            case Colon:
                if (key && !value) {
                    value = tk;
                    value_type = JsonPair_T;
                }
                break;
            case Comma:
                if (key && value && value_type && value_type != JsonPair_T) { // isn't colon by itself.
                    push_to_json(json, new_json_pair(key->value, value, value_type));
                    key = value = NULL;
                    value_type = Null;
                }
                break;
            case String_t:
                if (!key && !value) key = tk;
                else if (key && value && value_type == JsonPair_T) {
                    value = tk->value;
                    value_type = String;
                }
                break;
            case Int_t:
                if (key && value && value_type == JsonPair_T) {
                    value = tk->value;
                    value_type = Int;
                }
                break;
            case Number_t:
                if (key && value && value_type == JsonPair_T) {
                    value = tk->value;
                    value_type = Number;
                }
                break;
            case Lbracket:
                if (key && value && value_type == JsonPair_T) {
                    GenVec *new_gen_vec_ = convert_to_generic_vec(token_vec->original_data, token_vec->element_size, 0, max-1, false);
                    value = parse_json_array(new_gen_vec_, i);
                    value_type = JsonArray_T;
                    size_t new_index = new_gen_vec_->len - 1;
                    upper_bound = &new_index;
                    deallocate_generic_vec_and_tokens(new_gen_vec_);
                }
                break;
            case Rbracket:
                break;
            case Lcurly:
                if (key && value && value_type == JsonPair_T) { 
                    GenVec *new_gen_vec_ = convert_to_generic_vec(token_vec->original_data, token_vec->element_size, 0, max-1, false);
                    value = parse_json(new_gen_vec_, i);
                    size_t new_index = new_gen_vec_->len - 1;
                    upper_bound = &new_index;
                    value_type = Json_T;
                    deallocate_generic_vec_and_tokens(new_gen_vec_);
                }
                break;
            case Rcurly:
                if (key && value && value_type != JsonPair_T) { // isn't colon by itself.
                    push_to_json(json, new_json_pair(key->value, value, value_type));
                    key = value = NULL;
                    value_type = Null;
                }
                break;
            case NullType:
                if (key && value && value_type == JsonPair_T) {
                    value = tk->value;
                    value_type = Null;
                }
                break;
            case BoolTrue:
                if (key && value && value_type == JsonPair_T) {
                    value = tk->value;
                    value_type = Bool;
                }
                break;
            case BoolFalse:
                if (key && value && value_type == JsonPair_T) {
                    value = tk->value;
                    value_type = Bool;
                }
                break;
        }
        if (tk->type == Lcurly && i != 0) continue;
        token_vec->len = i + 1;
        if (tk->type == Rcurly) break;
    }
    return json;
}

GenVec *gen_vec_from_token_vec(TokenVec* vec) {
    GenVec *gen_vec = convert_to_generic_vec((void**)vec->tokens, sizeof(Token), 0, vec->len-1, false);
    return gen_vec;
}


int main() {
    // TODO: Fix tokenization for strings and double pointer free errors.
    //char json_str[] = "{\"key\":true, \"people\":{\"key_test\": \"value_test\", \"bees\": {\"two\": {\"eight\": 35}, \"nine\": 9}, \"test\": 1122}, \"orange\": [3, {\"nine\": true}, 8, 9]}"; // fix
    char json_str[] = "{\"data\": {\"id\": 1, {\"three\": 3}, \"values\": [100, 200, {\"nested\": {\"a\": 1, \"b\": 2}}, 300]}}";
    //char json_str[] = "{\"name\": \"Complex Example\", \"details\": {\"description\": \"This is a complex JSON example with various nested structures.\", \"metadata\": {\"version\": 1.0, \"tags\": [\"example\", \"complex\", \"test\"], \"created\": {\"date\": \"2023-05-23\", \"time\": \"14:00:00\"}}, \"active\": true, \"counts\": [10, 20, {\"subCount\": 5, \"subItems\": [1, 2, 3]}, 40]}, \"data\": [{\"id\": 1, \"type\": \"A\", \"values\": [100, 200, {\"nested\": {\"a\": 1, \"b\": 2}}, 300]}, {\"id\": 2, \"type\": \"B\", \"values\": [\"string\", null, true, false, {\"deeply\": {\"nested\": [\"array\", 123, {\"final\": \"object\"}]}}]}, {\"id\": 3, \"type\": \"C\", \"values\": []}], \"misc\": {\"emptyArray\": [], \"emptyObject\": {}, \"boolValues\": {\"trueValue\": true, \"falseValue\": false}, \"nullValue\": null, \"numberValues\": {\"integer\": 42, \"float\": 3.14159}, \"stringValues\": {\"empty\": \"\", \"nonEmpty\": \"non-empty string\"}}}";
    //char json_str[] = "{\"key\": [1, 2, [3, {\"1\": \"2\", \"2\": {\"5\": [1, 9, 3, 2, 1]}}]]}";
    TokenVec *vec = tokenize(json_str);
    print_token_vec(vec);
    GenVec *gen_vec = gen_vec_from_token_vec(vec);
    Json *json_output = parse_json(gen_vec, 0);
    print_json(json_output);
    printf("\n");
    deallocate_vec(vec);
    if (gen_vec->original_data) free(gen_vec->original_data);
    deallocate_generic_vec_and_tokens(gen_vec);
    deallocate_json(json_output);
}
