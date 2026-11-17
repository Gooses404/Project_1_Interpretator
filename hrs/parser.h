#pragma once
#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "commands.h"
#include "ast.h"

#define REDIR_ERROR 0b00001
#define EMPTY_BRACKET_ERROR 0b00010
#define UNCLOSED_BRACKET_ERROR 0b00100
#define CONTINUE_ERROR 0b01000
#define UNEXPECTED_EOF_ERROR  0b10000



ASTNode* parse_tokens(TokenArray *tokens);
ASTNode* parse_list(TokenArray *tokens);
ASTNode* parse_and_or(TokenArray *tokens);
ASTNode* parse_pipe(TokenArray *tokens);
ASTNode* parse_simple(TokenArray *tokens);
RedirArray *parse_single_redirection(RedirArray *redir,TokenArray *tokens);


void parser_test_mode();

#endif 
    