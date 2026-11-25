#include <stdio.h>
#include "commands.h"


int myth_pwd(char **argv){
    if (!argv || !argv[0]) {
        return 1;
    }
    char dir[PATH_MAX];
    if (getcwd(dir, sizeof(dir)) != NULL) {
        fprintf(stdout,"%s\n", dir);
        fflush(stdout);
    } else {
        return 1;
    }
    return 0;
}
int myth_exit(char **argv){
    if(config.interactive)
        write(STDOUT_FILENO,"logout\n",7);
    if (!argv || !argv[1]) {
        return config.shell_status;
    }
    char *check = NULL;
    strtol(argv[1],&check,10);

    if(check && *check != '\0'){
        fprintf(stderr,"mysh: exit: %s: numeric argument required\n",argv[1]);
        return 1;  
    }
    
    if(argv[2]){
        fprintf(stderr,"mysh: exit: too many arguments\n");
        return 1;
    }
    return atoi(argv[1]); 
}

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
            fprintf(stderr,"mysh: cd: %s: No such file or directory\n",argv[1]);
            break;
        case ENOTDIR:
            fprintf(stderr,"mysh: cd: %s: Not a directory\n",argv[1]);
            break;
        default:
            fprintf(stderr,"mysh: cd: Undefined error\n");
            break;
        }
        return 1;
    }
    return 0;
}

#define FLAG_N 0b01

int myth_echo(char **argv){
    if (!argv || !argv[0]) {
        return 1;
    }
    unsigned char flags = 0;
    // 00000000
    // 0:-n
    int i = 1;
    if (argv[i] != NULL && strcmp(argv[i], "-n") == 0) {
            flags |= FLAG_N;
            ++i;
        }
    while(argv[i] != NULL){

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