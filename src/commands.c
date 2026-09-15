
#include "commands.h"
char* Resize_char(char* input, int size){
    char* new_input = (char*)realloc(input, size * sizeof(char));
    if (new_input == NULL) {    
        free(input);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_input;
}
