#include "commands.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "signals.h"

#define SHELL_NAME "mysh"


int main(int argc, char* argv[]) {
    
    config.interactive = true;
    config.dump_tokens = false;
    config.dump_ast = false;

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
    //struct sigaction si;

    // sa.sa_handler = sigint_handler; 
    // sigemptyset(&sa.sa_mask);     
    // sa.sa_flags = SA_RESTART;
    // if (sigaction(SIGINT, NULL, NULL) == -1) {
    //     perror("Ошибка sigaction");
    //     return 1;
    // }
    TokenArray *head;
    ASTNode *root;
    char *text_of_input ;
   
    while(1){    // global loop for reading input
        free_tokens(head);
        text_of_input = read_input(0);
        if (text_of_input == NULL) {
            exit(0);                    // EOF \Ctrl-D 
        }
        head = tokenize(text_of_input);
        free(text_of_input);
                
        if(config.dump_tokens){
            print_tokens(head);
        }
        root = parse_tokens(head);
        if(config.dump_ast){
            print_ast(root, 0);
        }
        free(head);
        free_ast(root);

     //   execl("commads_list/myth_echo.c", "a", "b" ,NULL);
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    // int fd = open("input.txt", O_RDONLY);
    // if (fd == -1) {
    //     perror("Error opening file");
    //     exit(1);
    // }
    // int bytes_read = 0;
    // int count = 0, size = 128;
    // int l = 0 , w = 0, c = 0;
    // char buf[100];

    // while(1) {

    // bytes_read = read(fd, buf, 100);
    // if (bytes_read == -1) {
    //     perror("Error reading file");
    //     exit(1);
    // }
    // if (bytes_read == 0) {
    //     break; // End of file
    // }

    // // strncpy(text + count, buf, bytes_read);
    
    // for(int i = 0 ;i<bytes_read;++i){
    //     if(buf[i] == '\n') {
    //         l++;
    //     }
    //     if(buf[i] == ' ' || buf[i] == '\t') {
    //         w++;
    //         while(i < bytes_read && (buf[i] == ' ' || buf[i] == '\t')) {
    //             i++;
    //             c++;

    //         }
            
    //     }
    //     c++;
    // }

    // // count += bytes_read;
    
    
    // }
    // printf("Lines: %d, Words: %d, Characters: %d\n", l, w, c);
    // close(fd);
    return 0;
}
