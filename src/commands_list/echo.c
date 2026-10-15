#include "stdio.h"
#define FLAG_N 0b01

int myth_echo(char **argv){
    if (!argv || !argv[0]) {
        return 0;
    }
    unsigned char flags = 0;
    // 00000000
    // 0:-n
    int i = 1;
    while(argv[i] != NULL){
        if (strcmp(argv[i], "-n") == 0) {
            flags |= FLAG_N;
            ++i;
            continue;
        }
        
        write(STDOUT_FILENO,argv[i],strlen(argv[i]));
        if(argv[i + 1] != NULL){
            write(STDOUT_FILENO," ",1);
        }    
        ++i;
    }
    if(!(flags & FLAG_N)){
        write(STDOUT_FILENO,"\n",1);
    }

    return 0;
}