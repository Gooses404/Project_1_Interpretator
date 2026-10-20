#pragma once
#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "commands.h"
#include "ast.h"


ASTNode* parse_tokens(TokenArray *tokens);
ASTNode* parse_list(TokenArray *tokens);
ASTNode* parse_and_or(TokenArray *tokens);
ASTNode* parse_pipe(TokenArray *tokens);
ASTNode* parse_simple(TokenArray *tokens);



void parser_test_mode();

#endif 
    