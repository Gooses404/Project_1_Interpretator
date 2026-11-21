#include "commands.h"

int myth_exit(char **argv){
    write(STDOUT_FILENO,"logout\n",7);
    if (!argv || !argv[1]) {
        return config.last_status;
    }
    if(argv[2]){
        fprintf(stderr,"mysh: exit: %s: numeric argument required\n",argv[2]);
        return 1;
    }
    return atoi(argv[1]); 
}