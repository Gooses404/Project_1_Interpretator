
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
    
        while(1){    // по одному
        // читаем новое только если предыдущий buf уже полностью обработан
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
        while(buf_pos < res){
            // проверка размера
            if(bytes_read >= size_of_text_of_input - 3){
                size_of_text_of_input *= 2;
                text_of_input = Resize_char(text_of_input, size_of_text_of_input);
            }
            text_of_input[bytes_read++] = buf[buf_pos];
            if(buf[buf_pos++] == '\n' ){  // выход 
                goto out_of_loop;
            }
        
        }
        
    }
    out_of_loop:
    //free(buf);
    text_of_input[bytes_read] = '\0';
    return text_of_input ;

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
void reader_test_mode() {
    char *text_of_input = read_input(0);
    if (text_of_input == NULL) {
        exit(0); // Безопасный выход при EOF / Ctrl+D
    }
    printf("text_of_input: '%s'\nlen: %zu\n", text_of_input, strlen(text_of_input));
    free(text_of_input);
}
