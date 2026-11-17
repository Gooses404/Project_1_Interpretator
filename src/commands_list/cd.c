#include <stdio.h>
#include "commands.h"

#define FLAG_N 0b01

int myth_cd(char **argv){
    if (!argv || !argv[0]) {
        return 0;
    }
    unsigned char flags = 0;
    // 00000000
    // 0:-n
    int i = 1;
    if (strcmp(argv[i], "-n") == 0) {
            flags |= FLAG_N;
            ++i;
        }
    

    return 0;
}