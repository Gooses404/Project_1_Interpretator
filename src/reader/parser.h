#pragma once
#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "commands.h"
typedef enum {
    NODE_COMMAND,   
    NODE_PIPELINE,  
    NODE_AND,       
    NODE_OR,        
    NODE_SEQ,       
    NODE_BG,        
    NODE_SUBSHELL  
} ASTNodeType;

// Описание одного перенаправления: < file, > file, >> file
typedef struct RedirNode {
    TokenType type;            // TOKEN_REDIRECT_IN, TOKEN_REDIRECT_OUT, TOKEN_REDIRECT_D_OUT
    char *filename;            // Имя файла
    struct RedirNode *next;  // Следующее перенаправление (их может быть несколько: > out 2> err)
} RedirNode;

typedef struct ASTNode {
    ASTNodeType type;
    char **args;               // Массив строк аргументов: ["ls", "-la", NULL] для execvp!
    int args_count;
    RedirNode *redir; // Связный список перенаправлений этой команды

    // Для операторов (NODE_AND, NODE_OR, NODE_PIPELINE, NODE_SEQ, NODE_BG):
    struct ASTNode *left;
    struct ASTNode *right;

    // Для подоболочки (NODE_SUBSHELL):
    struct ASTNode *subshell_body; // поддерево внутри ()
} ASTNode;

ASTNode* parse_tokens(TokenArray *tokens);
ASTNode* parse_list(TokenArray *tokens);
ASTNode* parse_and_or(TokenArray *tokens);
ASTNode* parse_pipe(TokenArray *tokens);
ASTNode* parse_command(TokenArray *tokens);
RedirNode* parse_redirections(TokenArray *tokens, int *index);

void free_ast(ASTNode *node);



#endif 
    