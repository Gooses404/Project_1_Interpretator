#include "executor.h"
ASTNode *root;
int tmp_status;


int executer(ASTNode *ast){
    root = ast;
    tmp_status = 0;
    tmp_status = execute_node(ast);
}
int execute_node(ASTNode *ast){
    int result = 0;
    if(ast == NULL){
        return -1;
    }
    switch(ast->type){
        case NODE_COMMAND:
            return execute_command(ast);
        case NODE_PIPELINE:
            return execute_pipeline(ast);
        case NODE_AND:
            result = execute_node(ast->left);
            if (ast->right != NULL && result == 0) {
                result = execute_node(ast->right);
            }
            return result;
        case NODE_OR:
            result = execute_node(ast->left);
            if (ast->right != NULL && result != 0) {
                result = execute_node(ast->right);
            }   
            return result;
        case NODE_SEQ:
            result = execute_node(ast->left);
            if (ast->right != NULL) {
                result = execute_node(ast->right);
            }
            return result;

        case NODE_BG:
            return execute_bg(ast);
        case NODE_SUBSHELL:
            return execute_subshell(ast);
        default:
            fprintf(stderr, "Unknown AST node type: %d\n", ast->type);
            return -1;
    }
    
}
int execute_command(ASTNode *ast){
    if(ast->args == NULL){
        if(ast->redir != NULL){

        }else{
            config.last_status = 3;
        }
    }
    //Проверка на втроенную команду
    if (strcmp(ast->args->args_arr[0], "echo") == 0) {
        
    } else if (strcmp(ast->args->args_arr[0], "exit") == 0) {
        if(ast->args->args_count > 2){

        }
        tmp_status;
        free_ast(root);
        exit(tmp_status);

    } else if (strcmp(ast->args->args_arr[0], "cd") == 0) {

    }else if (strcmp(ast->args->args_arr[0], "pwd") == 0){

    }else if (strcmp(ast->args->args_arr[0], "help") == 0){

    }else if (strcmp(ast->args->args_arr[0], "jobs") == 0){

    }else if (strcmp(ast->args->args_arr[0], "kill") == 0){

    }else if (strcmp(ast->args->args_arr[0], "fg") == 0){
        if(!config.interactive){
            return -1;
        }

    }else if (strcmp(ast->args->args_arr[0], "bg") == 0){
        if(!config.interactive){
            return -1;
        }
    }else if (strcmp(ast->args->args_arr[0], "wait") == 0){

    }else{

    }
    
}
int redirect(ASTNode *ast);
int execute_pipeline(ASTNode *ast);
int execute_bg(ASTNode *ast);
int execute_subshell(ASTNode *ast);

