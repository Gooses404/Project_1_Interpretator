#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#define SIZE_OF_BUF 16
#define SIZE_OF_TEXT_OF_INPUT 100

char buf[SIZE_OF_BUF];
int buf_pos = 0;
ssize_t res = 0;

char* Resize_char(char* input, int size){
    char* new_input = (char*)realloc(input, size * sizeof(char));
    if (new_input == NULL) {    
        free(input);
        exit(1);
    }
    return new_input;
}

char* read_input(int fd){
    int bytes_read = 0 ; 
    int size_of_text_of_input = SIZE_OF_TEXT_OF_INPUT;
    char *text_of_input = malloc(size_of_text_of_input * sizeof(char));          
    bool is_backslash = false;

    while(1){
        if (buf_pos >= res) {
            buf_pos = 0;
            res = read(fd, buf, SIZE_OF_BUF);
        }
        
        if (res == -1) {
            if (errno == EINTR) continue;
            free(text_of_input);
            return NULL;
        }

        if (res == 0) { // EOF
            if (bytes_read == 0) {
                free(text_of_input);
                return NULL; 
            }
            break;
        }

        for (int i = buf_pos; i < res; ++i, ++buf_pos) {
            if (bytes_read >= size_of_text_of_input - 3) {
                size_of_text_of_input *= 2;
                text_of_input = Resize_char(text_of_input, size_of_text_of_input);
            }

            if (buf[i] != '\n' && is_backslash) {
                text_of_input[bytes_read++] = '\\';
                if (buf[i] == '\\') {
                    text_of_input[bytes_read++] = '\\';
                    is_backslash = false;
                    continue;
                }
            }

            if (is_backslash && buf[i] == '\n') {
                write(STDOUT_FILENO, "> ", 2);
            } else if (buf[i] != '\\') {
                text_of_input[bytes_read++] = buf[i];
            }

            if (buf[i] == '\n' && !is_backslash) {
                ++buf_pos;
                goto out_of_loop;
            }

            if (is_backslash) {
                is_backslash = false;
            }
            if (buf[i] == '\\' && !is_backslash) { 
                is_backslash = true;
            }
        }
    }

out_of_loop:
    if (is_backslash) {
        text_of_input[bytes_read++] = '\\';
    }
    text_of_input[bytes_read] = '\0';
    return text_of_input;
}

void reader_test_mode() {
    char *text_of_input = read_input(0);
    if (text_of_input == NULL) {
        printf("EOF reached, exiting\n");
        exit(0);
    }
    printf("text_of_input: '%s' (len %zu)\n", text_of_input, strlen(text_of_input));
    free(text_of_input);
}

int main() {
    while (1) {
        reader_test_mode();
    }
    return 0;
}
