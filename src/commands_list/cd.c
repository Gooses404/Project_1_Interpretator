#include <stdio.h>
#include "commands.h"

#define FLAG_N 0b01

int myth_cd(char **argv){
    
    if (!argv || !argv[0]) {
        return 1;
    }
    if (!argv[1]) {
        if(chdir(getenv("HOME")) != 0)
            return 1;
        return 0;
        
    }
    if(argv[2]){
        fprintf(stderr,"mysh: cd: %s: too many arguments\n",argv[2]);
        return 1;
    }
    if(chdir(argv[1]) != 0){
        switch (errno)
        {
        case ENOENT:
            fprintf(stderr,"mysh: cd: %s: No such file or directory\n",argv[2]);
            break;
        case ENOTDIR:
            fprintf(stderr,"mysh: cd: %s: Not a directory\n",argv[2]);
            break;
        default:
            break;
        }
        return 1;
    }
    return 0;
}