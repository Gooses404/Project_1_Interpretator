
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
Token* Resize_Token_Array(Token* token, int size){
    Token* new_arr = (Token*)realloc(token, size * sizeof(Token));
    if (new_arr == NULL) {    
        free(token);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_arr;
}
void lexer_test_mode(){
        char *text_of_input ;
        TokenArray* head;
        printf("START_OF_PROGRAMM\n");
        text_of_input = read_input(0);
        if (text_of_input == NULL) {
            exit(0);                    // EOF \Ctrl-D 
        }
        head = tokenize(text_of_input);
        free(text_of_input);
        print_tokens(head);
        destroy_tokens(head);
}

char* read_input(int fd){
    printf("Start of reading\n");
    int bytes_read = 0 ,                size_of_text_of_input = 100,    size_of_buf = 16;
    char *text_of_input = malloc(size_of_text_of_input * sizeof(char));          
    ssize_t res = 0;
    char *buf = malloc(size_of_buf * sizeof(char));
        while(1){    // по одному

        res = read(fd, buf, size_of_buf);
        
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
        for(int i = 0 ; i < res ; ++i){
        
            text_of_input[bytes_read++] = buf[i];        
            
            if(bytes_read >= size_of_text_of_input - 2){
                size_of_text_of_input *= 2;
                text_of_input = Resize_char(text_of_input, size_of_text_of_input);
            }
            
            if(buf[i] == '\n'){ // запись перед прекращ ввода
                //  text_of_input[bytes_read++] = buf[i];   
                goto out_of_loop;
                
            }
        }
        
    }
    out_of_loop:
    free(buf);
    text_of_input[bytes_read] = '\0';
    //printf("End_of_reading %s ||a %d\n",text_of_input, strlen(text_of_input));
    return text_of_input ;

}

