#pragma once 
#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "ast.h"
#include "commands.h"

int read_bfs(ASTNode *ast);
int execute_command(ASTNode *ast);

#endif