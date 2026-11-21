#include <stdio.h>
#include "commands.h"


int myth_pwd(char **argv){
    if (!argv || !argv[0]) {
        return 1;
    }
    char dir[PATH_MAX];
    if (getcwd(dir, sizeof(dir)) != NULL) {
        fprintf(stdout,"%s\n", dir);
    } else {
        return 1;
    }
    return 0;
}