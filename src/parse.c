#include "tokenizer.h"
#include "json.h"
#include "iterator.h"
#include "string.h"
#include "vec.h"

bool is_kv_separator(const Token *token) {
    return (token != NULL && token->type == Colon);
}

bool is_array_separator(const Token *token) {
    return (token != NULL && token->type == Comma);
}

bool is_key(const Token *curr, Iterator *it) {
    bool curr_is_string = (curr != NULL && curr->type == String_Type);
    bool next_is_kv_sep = is_kv_separator(peek(it));
    return (curr_is_string && next_is_kv_sep);
}

bool token_is_keyword(const Token *token) {
    return (token->type == BoolTrue || 
            token->type == BoolFalse ||
            token->type == Null_T);
}

bool is_array(const Token *token) {
    return token->type == Lbracket;
}

bool is_array_break(const Token *token) {
    return token->type == Rbracket;
}

bool is_object(const Token *token) {
    return token->type == Lcurly;
}

bool is_object_break(const Token *token) {
    return token->type == Rcurly;
}

bool is_string_or_numeric(const Token *token) {
    return (token->type == String_Type || token->type == Number_T);
}

Identifier get_attr_type(const Token *token) {
    if (token_is_keyword(token)) {
        if (token->type == BoolTrue || token->type == BoolFalse) {
            return Bool;
        } else if (token->type == Null_T) {
            return Null;
        }
    } else if (is_string_or_numeric(token)) {
        if (token->type == String_Type) {
            return String_T;
        } else if (token->type == Number_T) {
            return Number;
        }
    }
    return Null;
}

Attribute to_attr(void *attr, Identifier type, bool *attr_found) {
    Attribute attribute = new_attr(attr, type);
    *attr_found = true;
    return attribute;
}

bool has_valid_array_separation(const Token *token) {
    return (token != NULL && (token->type == Comma || token->type == Rbracket));
}

bool has_valid_object_separation(const Token *token) {
    return (token != NULL && (token->type == Comma || token->type == Rcurly));
}

void *get_kwd_or_literal_value(const Token *token) {
    if (token->type == String_Type || 
        token->type == Number_T) {
        return token->value;
    } else if (token->type == BoolTrue || token->type == BoolFalse) {
        bool *bool_value = (bool*)malloc(sizeof(bool));
        *bool_value = token->type == BoolTrue;
        return bool_value;
    } else if (token->type == Null_T) {
        return NULL;
    }
    return NULL;
}

Object *parse(Iterator *it);

Array *parse_array(Iterator *it) {
    Token *curr;
    Array *array = new_heap_array();
    Attribute attr;
    bool attr_found = false;
    for (curr = next(it); curr != NULL; curr = next(it)) {
        if (is_array(curr)) {
            Array *arr = parse_array(it); 
            attr = to_attr(arr, Array_T, &attr_found);
        } else if ((token_is_keyword(curr) || is_string_or_numeric(curr)) &&
                   has_valid_array_separation(peek(it))) {
            attr = to_attr(get_kwd_or_literal_value(curr), get_attr_type(curr), &attr_found);
        } else if (is_object(curr)) {
            Object *object = parse(it);
            attr = to_attr(object, Object_T, &attr_found);
        } else if (is_array_break(curr)) {
            break;
        }
        if (attr_found) {
            push_to_array(array, &attr);
            attr_found = false;
        } 
    }
    return array;
}

Object *parse(Iterator *it) {
    Object *object = new_heap_object();
    for (Token *key_ptr = next(it); key_ptr != NULL; key_ptr = next(it)) {
        if (is_object_break(key_ptr)) { // breaks when corresp closing brace.
            break;
        } else if (!is_key(key_ptr, it)) {
            continue;
        } else {
            next(it); // comma
        }
        String *key = key_ptr->value;
        Attribute value;
        bool value_found = false;
        Token *curr = next(it); // start token from value found in the key/value pair.
        if (curr == NULL) {
            break;
        } else if (is_object(curr)) {
            Object *next_obj = parse(it);
            value = to_attr(next_obj, Object_T, &value_found);
        } else if (is_array(curr)) {
            Array *next_arr = parse_array(it);
            value = to_attr(next_arr, Array_T, &value_found);
        } else if ((token_is_keyword(curr) || is_string_or_numeric(curr)) &&
                   has_valid_object_separation(peek(it))) {
            value = to_attr(get_kwd_or_literal_value(curr), get_attr_type(curr), &value_found);
        }
        if (value_found) {
            Json json = new_json(key, &value);
            push_to_object(object, &json);
            value_found = false;
            // TODO: throw error if peeked isn't , or }.
        }
    }
    return object;
}

int main() {
    String json_str = new_string("{\"name\":\"Mundar\",\"age\":19.2,\"mentalAge\":4,\"skills\":[{\"type\":\"language\",\"subtype\":\"spoken\",\"description\":\"English\"},{\"type\":\"language\",\"subtype\":\"coding\",\"description\":\"Python\"},{\"type\":\"language\",\"subtype\":\"coding\",\"description\":\"C\"},{\"type\":\"instrument\",\"subtype\":null,\"description\":\"Violin\"},{\"type\":\"instrument\",\"subtype\":null,\"description\":\"Piano\"}]}");
    Vec token_vec = tokenize(&json_str);
    Iterator it = iter_from_vec(&token_vec);
    Object *json_object = parse(&it); // parses iterator of tokens to Json object.
    print_object(json_object);
    printf("\n");
//     print_token_vec(&token_vec);
    Attribute array = json_object->get(json_object, "skills");
    Attribute json_0 = array.index(&array, 0);
    Attribute type_0 = json_0.get(&json_0, "type");
    Attribute subtype_0 = json_0.get(&json_0, "subtype");
    Attribute description = json_0.get(&json_0, "description");
    print_attr(&json_0);
    printf("\n");
    print_attr(&type_0);
    printf("\n");
    print_attr(&subtype_0);
    printf("\n");
    print_attr(&description);
    printf("\n");
    free_object(json_object);
    free_token_vec(&token_vec);
    reset_str(&json_str);
    return 0;
}
