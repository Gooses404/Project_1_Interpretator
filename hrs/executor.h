#pragma once 
#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "ast.h"
#include "commands.h"
int executer(ASTNode *ast);
int execute_command(ASTNode *ast);
int execute_node(ASTNode *ast);
int redirect(ASTNode *ast);
int execute_pipeline(ASTNode *ast);
int execute_bg(ASTNode *ast);
int execute_subshell(ASTNode *ast);

///////////////////////////////////
int myth_exit(char **argv);
int myth_echo(char **argv);
int myth_cd(char **argv);
int myth_pwd(char **argv);
int myth_help(char **argv);
int myth_jobs(char **argv);
int myth_fg(char **argv);
int myth_bg(char **argv);
int myth_kill(char **argv);
int myth_wait(char **argv);



#endif