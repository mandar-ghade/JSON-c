#include "vec.h"
#include "string.h"
#include "iterator.h"
#include "tokenizer.h"
#include "json.h"
#include "parse.h"
#include <stdio.h>

char *read_file_to_buffer(FILE *f_ptr, char *file_name, char *buffer, size_t *buff_size) {
    printf("File name: ");
    scanf("%s", file_name);
    printf("\n");
    f_ptr = fopen(file_name, "r");
    ssize_t bytes_read = getdelim(&buffer, buff_size, '\0', f_ptr);
    return buffer;
}

int main() {
    FILE *f_ptr;
    size_t FILE_NAME_BUFF = 50;
    String buffer_str;
    char *buffer = NULL;
    size_t buff_size;
    char file_name[FILE_NAME_BUFF];
    buffer = read_file_to_buffer(f_ptr, file_name, buffer, &buff_size);
    buffer_str = new_string(buffer);
    Vec token_vec = tokenize(&buffer_str);
    Iterator it = iter_from_vec(&token_vec);
    Object *json_object = parse(&it);
    print_object(json_object);
    printf("\n");
    free_object(json_object);
    free_token_vec(&token_vec);
    reset_str(&buffer_str);
    free(buffer);

}
