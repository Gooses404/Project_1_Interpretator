#include "commands.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "signals.h"    
#include "executor.h"    

#define SHELL_NAME "mysh"

ShellConfig config = { .interactive = true, .dump_tokens = false, .dump_ast = false, .command_str = NULL };     
int main(int argc, char* argv[]) {
    char dir[PATH_MAX];
    config.interactive = isatty(STDIN_FILENO);
    config.dump_tokens = false;
    config.dump_ast = false;
    config.last_status = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--dump-tokens") == 0) {
            config.dump_tokens = true;
        } else if (strcmp(argv[i], "--dump-ast") == 0) {
            config.dump_ast = true;
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            config.command_str = argv[++i];
            config.interactive = false;
        }
    }
    TokenArray *head = NULL;
    ASTNode *root = NULL;
    char *text_of_input ;

    while(1){    // global loop for reading input
        if(config.interactive){
            write(STDOUT_FILENO,"myth:",5);
            if (getcwd(dir, sizeof(dir)) != NULL) {
                write(STDOUT_FILENO,dir, strlen(dir));
                write(STDOUT_FILENO,"$ ", 2);
             } else {
                return 1;
    }

        }
        config.last_status = 0;
        if(config.command_str){
            text_of_input = strdup(config.command_str);
        }else{
            text_of_input = read_input(0);
        }

        if (text_of_input == NULL) {
            exit(config.last_status);                    // EOF \Ctrl-D         
        }
        head = tokenize(&text_of_input);
        if(text_of_input)
            free(text_of_input);
                
        if(head == NULL){
            if(config.interactive){
                continue;
            }
            if(config.last_status == 2)
                exit(2);
        }
        if(config.dump_tokens){
            print_tokens(head);
        }

        root = parse_tokens(head);
        free_tokens(head);
        if(root == NULL){
            if(config.interactive){
                continue;
            }
            if(config.last_status == 2)
                exit(2);
        }
        if(config.dump_ast){
            print_ast(root, 0);
        }
        executer(root);

        free_ast(root);





        if(config.command_str){
            return config.last_status;    
        }
    }

    return 0;   
}
