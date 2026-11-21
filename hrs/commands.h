#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <error.h>
#include <errno.h>
#include <sys/wait.h>

typedef struct {
    bool interactive;       
    bool dump_tokens;         
    bool dump_ast;    
    int last_status; 
    int syntax_error;       
    const char *command_str;
} ShellConfig;
extern ShellConfig config;
#define SIZE_OF_BUF 16
#define SIZE_OF_TEXT_OF_INPUT 100

#define PATH_MAX 256


char* Resize_char(char* input, int size);

char* read_input(int fd);
char *merge_input_strings(char *s1, char *s2 );
void reader_test_mode();

//////////////////////
//void myth_print_error(int errnum);
#endif
