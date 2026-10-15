#pragma once
#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "commands.h"

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

typedef struct ASTNode {
    ASTNodeType type;
    char **args;               
    int args_count;
    RedirArray *redir; 

    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode* parse_tokens(TokenArray *tokens);
ASTNode* parse_list(TokenArray *tokens);
ASTNode* parse_and_or(TokenArray *tokens);
ASTNode* parse_pipe(TokenArray *tokens);
ASTNode* parse_simple(TokenArray *tokens);


ASTNode* create_node(ASTNodeType type);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int level);

RedirArray* create_redir_array();
bool is_redir(TokenType type);
RedirArray* add_redir_node(RedirArray* array, TokenType type, const char *filename);
RedirNode* resize_redir_array(RedirNode* redir, int size);

void parser_test_mode();

#endif 
    