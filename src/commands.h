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
void lexer_test_mode();

char* read_input();



#endif
