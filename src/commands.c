
#include "commands.h"
#include "lexer.h"
char* Resize_char(char* input, int size){
    char* new_input = (char*)realloc(input, size * sizeof(char));
    if (new_input == NULL) {    
        free(input);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_input;
}
void lexer_test_mode(){
        char *text_of_input ;
        Token* head;
        printf("START_OF_PROGRAMM\n");
        text_of_input = read_input();
        head = tokenize(text_of_input);
        print_tokens(head);
        destroy_tokens(head);
}
