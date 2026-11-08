
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


char buf[SIZE_OF_BUF];
int buf_pos = 0;
ssize_t res = 0;
char* read_input(int fd){
    int bytes_read = 0 ; 
    int size_of_text_of_input = SIZE_OF_TEXT_OF_INPUT;
    char *text_of_input = malloc(size_of_text_of_input * sizeof(char));          
    if(text_of_input == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    bool is_backslash = false;
    bool is_single_quote = false;
    
        while(1){    // по одному
        if(buf_pos >= res){
            buf_pos = 0;
            res = read(fd, buf, SIZE_OF_BUF);
        }
        
        if(res == -1 ){
            if (errno == EINTR) {
                continue; // Прерывание сигналом 
            }
            free(text_of_input);
           // free(buf);
            return NULL;

        }
        if( res == 0 ){ // EOF
            if (bytes_read == 0) {
                free(text_of_input);
               // free(buf);
                return NULL; 
            }
            
            break;
        }
        // buf pos всегда на следующем к концу цикла
        while( buf_pos < res){
            // проверка размера
            if(bytes_read >= size_of_text_of_input - 2){
                size_of_text_of_input *= 2;
                text_of_input = Resize_char(text_of_input, size_of_text_of_input);
            }
            
            
            if(is_backslash ){
                if(buf[buf_pos] != '\n'){ // записываем \ только на следующей итерации
                    text_of_input[bytes_read++] = '\\';
                    if(buf[buf_pos] == '\\'){ // если двойной то записываем оба
                        text_of_input[bytes_read++] = '\\';
                        is_backslash = false;
                        ++buf_pos;
                        continue;
                    }
                }
                if((buf[buf_pos] == '\n') ){ // не записываем \n в случае is_backslash = true ; пока не пишем бэкслэш
                    write(STDOUT_FILENO,">",1);
                    ++buf_pos;
                    continue;
                }
                is_backslash = false;
            }else{
                if(buf[buf_pos] == '\n' ){  // выход 
                    text_of_input[bytes_read++] = buf[buf_pos++];
                    goto out_of_loop;
                }else if(buf[buf_pos] == '\\'){
                    is_backslash = true; // возможность продолжить команду 

                }
            }
        if(buf[buf_pos] != '\\'){
            text_of_input[bytes_read++] = buf[buf_pos];
        }
        ++buf_pos;
            
        }
        
    }
    out_of_loop:
    //free(buf);
    text_of_input[bytes_read] = '\0';
    return text_of_input ;

}

void reader_test_mode() {
    char *text_of_input = read_input(0);
    if (text_of_input == NULL) {
        exit(0); // Безопасный выход при EOF / Ctrl+D
    }
    printf("text_of_input: '%s'\nlen: %zu\n", text_of_input, strlen(text_of_input));
    free(text_of_input);
}

char* merge_input_strings(char* s1, char* s2) {
    if (!s1 || !s2) {
        free(s2);
        return NULL;
    }
    size_t n1 = strlen(s1);
    size_t n2 = strlen(s2);
    char* s3 = (char*)realloc(s1, n1 + n2 + 1);
    if (!s3) {
        free(s2);
        return NULL;
    }
    memcpy(s3 + n1, s2, n2 + 1); // сохраняем \n в конце s1
    free(s2);
    return s3;
}
