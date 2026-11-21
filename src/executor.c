#include "executor.h"
ASTNode *root;
int tmp_status;
pid_t pr_pid;
int saved_stdin, saved_stdout; 

int executer(ASTNode *ast){
    root = ast;
    tmp_status = 0;
    saved_stdin = STDIN_FILENO;
    saved_stdout = STDIN_FILENO;
    tmp_status = execute_node(ast);
    return tmp_status;
}
int execute_node(ASTNode *ast){
    int result = 0;
    
    if(ast == NULL){
        return 0;
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
            fprintf(stderr, "mysh: executor: Unknown AST node type: %d\n", ast->type);
    }
    return result;
}
int execute_command(ASTNode *ast){
    
    if(ast->args == NULL){
        if(ast->redir != NULL){
        
        }else{
            config.last_status = 3;
            return 1;
        }
    }
    //Проверка на втроенную команду
    if (strcmp(ast->args->args_arr[0], "echo") == 0) {
        if(redirect(ast)){
            return 1;
        }
        
        tmp_status = myth_echo(ast->args->args_arr);
        
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }
        return tmp_status;
    } else if (strcmp(ast->args->args_arr[0], "exit") == 0) {
        
        tmp_status = myth_exit(ast->args->args_arr);
        
        free_ast(root);
        exit(tmp_status);

    } else if (strcmp(ast->args->args_arr[0], "cd") == 0) {
        if(redirect(ast)){
            return 1;
        }
        
        tmp_status = myth_cd(ast->args->args_arr);
        
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }
        return tmp_status;
    }else if (strcmp(ast->args->args_arr[0], "pwd") == 0){
        if(redirect(ast)){
            return 1;
        }
        tmp_status = myth_pwd(ast->args->args_arr);
        
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }
        return tmp_status;
    }else if (strcmp(ast->args->args_arr[0], "help") == 0){
        if(redirect(ast)){
            return 1;
        }
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }
    }else if (strcmp(ast->args->args_arr[0], "jobs") == 0){
        if(redirect(ast)){
            return 1;
        }
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }
    }else if (strcmp(ast->args->args_arr[0], "kill") == 0){
        if(redirect(ast)){
            return 1;
        }
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }
    }else if (strcmp(ast->args->args_arr[0], "fg") == 0){
        if(!config.interactive){
            return 1;
        }
        if(redirect(ast)){
            return 1;
        }
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }

    }else if (strcmp(ast->args->args_arr[0], "bg") == 0){
        if(!config.interactive){
            return 1;
        }
        if(redirect(ast)){
            return 1;
        }
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }
    }else if (strcmp(ast->args->args_arr[0], "wait") == 0){
        if(redirect(ast)){
            return 1;
        }
        if(ast->redir){
            dup2(saved_stdin, STDIN_FILENO);
            dup2(saved_stdout, STDOUT_FILENO);
        }
    }else{
        pr_pid = fork();
        if(pr_pid < 0 ){
            config.last_status = errno;
            fprintf(stderr,"mysh: executor: fork error %s\n",strerror(errno));
            return 1;
        }else if(pr_pid == 0){
            if(redirect(ast)){
                _exit(1);
            }   
            if(execvp(ast->args->args_arr[0], ast->args->args_arr) != 0){
                fprintf(stderr,"%s\n",ast->args->args_arr[0]);
                //config.last_status = errno;
                fprintf(stderr,"mysh: executor: execvp error %s\n",strerror(errno));
                _exit(errno);
           }
        }
        tmp_status = waitpid(pr_pid,&config.last_status,0);
        // if(!tmp_status){
        // }
    }
    return tmp_status;
}
int redirect(ASTNode *ast){
    if(ast == NULL)
        return 1;
    if(ast->redir == NULL)
        return 0;
    int fd;
    
    for(int i = 0 ;i < ast->redir->cur_index ; i++){

        switch(ast->redir->head[i].type){
            case TOKEN_REDIRECT_IN:
                fd = open(ast->redir->head[i].filename, O_RDONLY);
                if(fd < 0){
                    config.last_status = errno;
                    fprintf(stderr,"mysh:  %s: %s\n",ast->redir->head[i].filename,strerror(errno));
                    return 1;
                }
                if(dup2(fd ,STDIN_FILENO)){
                    config.last_status = errno;
                    fprintf(stderr,"mysh: %s: %s\n",ast->redir->head[i].filename,strerror(errno));
                    close(fd);
                    return 1;
                }
                break;
            case TOKEN_REDIRECT_OUT:
                fd = open(ast->redir->head[i].filename, O_WRONLY | O_CREAT | O_TRUNC ,0644);
                if(fd < 0){
                    config.last_status = errno;
                    fprintf(stderr,"mysh: %s: %s\n",ast->redir->head[i].filename,strerror(errno));
                    close(fd);
                    return 1;
                }
                if(dup2(fd ,STDOUT_FILENO)){
                    config.last_status = errno;
                    fprintf(stderr,"mysh: %s: %s\n",ast->redir->head[i].filename,strerror(errno));
                    close(fd);
                    return 1;
                }       
                break;
            case TOKEN_REDIRECT_D_OUT:
                fd = open(ast->redir->head[i].filename, O_WRONLY | O_CREAT | O_APPEND ,0644);
                if(fd < 0){
                    config.last_status = errno;
                    fprintf(stderr,"mysh: %s: %s\n",ast->redir->head[i].filename,strerror(errno));
                    close(fd);
                    return 1;
                }
                if(dup2(fd ,STDOUT_FILENO)){
                    config.last_status = errno;
                    fprintf(stderr,"mysh: %s: %s\n",ast->redir->head[i].filename,strerror(errno));
                    close(fd);
                    return 1;
                }
                break;

        }
    }   
    close(fd);
    return 0;

}
int execute_pipeline(ASTNode *ast){
    if(ast == NULL)
        return 1;
    return 0;
}
int execute_bg(ASTNode *ast){
    if(ast == NULL)
        return 1;
    return 0;
}
int execute_subshell(ASTNode *ast){
    if(ast == NULL)
        return 1;
    pr_pid = fork();
    if(pr_pid < 0 ){
        config.last_status = errno;
        fprintf(stderr,"mysh: executor: fork error %s\n",strerror(errno));
        return 1;
    }else if(pr_pid == 0){
        if(redirect(ast) != 0){
            _exit(1);    
        }
        tmp_status = execute_node(ast->left);
        _exit(tmp_status);
        }
    tmp_status = waitpid(pr_pid,&config.last_status, 0);
    return tmp_status;

}

