#pragma once
#ifndef AST_H
#define AST_H
#include "lexer.h"
#define R_ARRAY_INITIAL_SIZE 2
#define ARG_ARRAY_INITIAL_SIZE 4
typedef enum {
    NODE_COMMAND,
    NODE_PIPELINE,  
    NODE_AND,       
    NODE_OR,        
    NODE_SEQ,       
    NODE_BG,        
    NODE_SUBSHELL  
} ASTNodeType;

//  < file, > file, >> file
typedef struct RedirNode {
    TokenType type;            // TOKEN_REDIRECT_IN, TOKEN_REDIRECT_OUT, TOKEN_REDIRECT_D_OUT
    char *filename; 
} RedirNode;

typedef struct RedirArray {
    RedirNode *head;
    int cur_index;
    int cur_size;
}RedirArray; 

typedef struct ArgsArray {
    char **args_arr;
    int args_count;
    int args_size;
}ArgsArray; 

typedef struct ASTNode {
    ASTNodeType type;
    //char **args;               
    //int args_count;
    RedirArray *redir;
    ArgsArray *args;

    struct ASTNode *left; // subshell тоже тут
    struct ASTNode *right;
} ASTNode;

//////////////////////////// AST FUNCS
ASTNode* create_node(ASTNodeType type);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int level);
///////////////////////////// REDIRS
RedirArray* create_redir_array();
bool is_redir(TokenType type);
RedirArray* add_redir_node(RedirArray* array, TokenType type, const char *filename);
RedirNode* resize_redir_array(RedirNode* redir, int size);
//////////////////////////// ARGS
ArgsArray* create_args_array();
char** resize_args_array(char** args, int size); 
#endif