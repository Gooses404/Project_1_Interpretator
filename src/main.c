#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "commands.h"
#include "lexer.h"

int main() {
    int bytes_read = 0 ,size_of_text_of_input = 100;
    char *text_of_input ;
    while(1){    // global loop for reading input
        text_of_input = read_input();
        print_tokens(tokenize(text_of_input));
    
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    // int fd = open("input.txt", O_RDONLY);
    // if (fd == -1) {
    //     perror("Error opening file");
    //     exit(1);
    // }
    // int bytes_read = 0;
    // int count = 0, size = 128;
    // int l = 0 , w = 0, c = 0;
    // char buf[100];

    // while(1) {

    // bytes_read = read(fd, buf, 100);
    // if (bytes_read == -1) {
    //     perror("Error reading file");
    //     exit(1);
    // }
    // if (bytes_read == 0) {
    //     break; // End of file
    // }

    // // strncpy(text + count, buf, bytes_read);
    
    // for(int i = 0 ;i<bytes_read;++i){
    //     if(buf[i] == '\n') {
    //         l++;
    //     }
    //     if(buf[i] == ' ' || buf[i] == '\t') {
    //         w++;
    //         while(i < bytes_read && (buf[i] == ' ' || buf[i] == '\t')) {
    //             i++;
    //             c++;

    //         }
            
    //     }
    //     c++;
    // }

    // // count += bytes_read;
    
    
    // }
    // printf("Lines: %d, Words: %d, Characters: %d\n", l, w, c);
    // close(fd);
    return 0;
}
