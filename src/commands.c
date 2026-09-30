
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
        TokenList* head;
        printf("START_OF_PROGRAMM\n");
        text_of_input = read_input();
        if (text_of_input == NULL) {
            exit(0);                    // EOF \Ctrl-D 
        }
        head = tokenize(text_of_input);
        free(text_of_input);
        print_tokens(head);
        destroy_tokens(head);
}

char* read_input(){
    printf("Start of reading\n");
    int bytes_read = 0 ,                size_of_text_of_input = 100;
    char *text_of_input = malloc(size_of_text_of_input * sizeof(char)),          
        symbol[1];
        ssize_t res = 0;
        
        while(1){    // по одному

        res = read(0, symbol, 1);
        
        if(res == -1 ){
            if (errno == EINTR) {
                continue; // Прерывание сигналом 
            }
            free(text_of_input);
            return NULL;

        }
        if( res == 0 ){ // EOF
            if (bytes_read == 0) {
                free(text_of_input);
                return NULL; 
            }
            
            break;
        }
        text_of_input[bytes_read++] = symbol[0];        
        if(symbol[0] == '\n'){ // запись перед прекращ ввода
            //  text_of_input[bytes_read++] = symbol[0];   
            break;
        }

        if(bytes_read >= size_of_text_of_input - 1){
            size_of_text_of_input *= 2;
            text_of_input = Resize_char(text_of_input, size_of_text_of_input);
        }      
        ////printf(" %c",symbol[0]);    
    }
    if(bytes_read + 1  >= size_of_text_of_input - 1){
        size_of_text_of_input *= 2;
        text_of_input = Resize_char(text_of_input, size_of_text_of_input);
    }
    text_of_input[bytes_read] = '\0';
    //printf("End_of_reading %s ||a %d\n",text_of_input, strlen(text_of_input));
    return text_of_input ;

}

