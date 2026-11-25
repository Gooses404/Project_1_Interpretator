#pragma once 
#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "ast.h"
#include "commands.h"

#define SIZE_OF_PIPES 2

int executer(ASTNode *ast);
int execute_command(ASTNode *ast, bool is_bg);
int execute_node(ASTNode *ast, bool is_bg);
int execute_subshell(ASTNode *ast, bool is_bg);
int execute_pipeline(ASTNode *ast,bool is_bg);
int redirect(ASTNode *ast);

int return_std(ASTNode* ast);
int decode_wait(pid_t pid);
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