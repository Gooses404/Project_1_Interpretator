#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
#include <errno.h>
typedef struct {
    bool interactive;       
    bool dump_tokens;         
    bool dump_ast;            
    const char *command_str;  
} ShellConfig;

char* Resize_char(char* input, int size);

char* read_input(int fd);



#endif
