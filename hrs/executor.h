#pragma once 
#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "ast.h"
#include "commands.h"

int execute_command(ASTNode *ast);
int execute_node(ASTNode *ast);
int redirect(ASTNode *ast);
int execute_pipeline(ASTNode *ast);
int execute_bg(ASTNode *ast);
int execute_subshell(ASTNode *ast);
#endif