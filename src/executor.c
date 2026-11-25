#include "executor.h"
ASTNode *root;
int tmp_status;
pid_t pr_pid;
int saved_stdin, saved_stdout; 

int executer(ASTNode *ast){
    root = ast;
    tmp_status = 0;
    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);
    tmp_status = execute_node(ast, false);
    close(saved_stdin);
    close(saved_stdout);
    return tmp_status;
}
int execute_node(ASTNode *ast, bool is_bg){
    int result = 0;
    
    if(ast == NULL){
        return 0;
    }
    switch(ast->type){
        case NODE_COMMAND:
            return execute_command(ast, is_bg);
        case NODE_PIPELINE:
            return execute_pipeline(ast, is_bg);
        case NODE_AND:
            //Если is_bg то создаем дочерний и делаем все в нем
            if(is_bg){
                pr_pid = fork();
                if(pr_pid < 0){
                    config.last_status = errno;
                    fprintf(stderr,"mysh: executor: fork error %s\n",strerror(errno));
                    return 1;
                }else if(pr_pid == 0){
                    setpgid(0,0);
                    result = execute_node(ast->left, false);
                    if (ast->right != NULL && result == 0 && config.shell_status != 3) {
                        result = execute_node(ast->right, false);
                    }
                    _exit(result);

                }
                setpgid(pr_pid,pr_pid);

                return 0 ;
            }else{
                result = execute_node(ast->left, false);
                if (ast->right != NULL && result == 0 && config.shell_status != 3) {
                    result = execute_node(ast->right, false);
                }
                return result;
            }
        case NODE_OR:
            //Если is_bg то создаем дочерний и делаем все в нем
            if(is_bg){
                pr_pid = fork();
                if(pr_pid < 0){
                    config.last_status = errno;
                    fprintf(stderr,"mysh: executor: fork error %s\n",strerror(errno));
                    return 1;
                }else if(pr_pid == 0){
                    setpgid(0,0);
                    result = execute_node(ast->left, false);
                    if (ast->right != NULL && result != 0 && config.shell_status != 3) {
                        result = execute_node(ast->right, false);
                    }
                    _exit(result);
                }
                setpgid(pr_pid,pr_pid);
                //add job
                return 0 ;
            }else{
                result = execute_node(ast->left, false);
                if (ast->right != NULL && result != 0 && config.shell_status != 3) {
                    result = execute_node(ast->right, false);
                }
                return result;
            }
        case NODE_SEQ:
            result = execute_node(ast->left, false);
            if (ast->right != NULL) {
                result = execute_node(ast->right, is_bg);
            }
            return result;

        case NODE_BG:
            result = execute_node(ast->left, true);
        
            if(ast->right)
                result = execute_node(ast->left, is_bg); // спускаем is_bg ниже для правой ветви а левую в фоне

            return result;
        case NODE_SUBSHELL:
            return execute_subshell(ast, is_bg);
        default:
            fprintf(stderr, "mysh: executor: Unknown AST node type: %d\n", ast->type);
    }
    return result;
}
int execute_command(ASTNode *ast, bool is_bg){
    
    if(ast->args == NULL){
        if(ast->redir != NULL){ //создаем файл
            pr_pid = fork();
            if(pr_pid < 0 ){
                config.last_status = errno;
                fprintf(stderr,"mysh: executor: fork error %s\n",strerror(errno));
                return 1;
            }else if(pr_pid == 0){
                _exit(redirect(ast));
            }
            if(!is_bg){
                tmp_status = decode_wait(pr_pid);
                return tmp_status;
            }
            return 0 ;
        }else{
            config.shell_status = 3;
            return 1;
        }
    }
    if(is_bg){
        // при is_bg в любом случае делаем форк и в родителе сразу выходим
        pr_pid = fork();
        if(pr_pid < 0 ){
            config.last_status = errno;
            fprintf(stderr,"mysh: executor: fork error %s\n",strerror(errno));
            return 1;
        }else if(pr_pid > 0){
            setpgid(pr_pid, pr_pid);
            //add job
            return 0;
        }
        // отбираем терминал у фонового процесса 
        config.interactive = false;
        setpgid(0 , 0);
    }
    //Проверка на втроенную команду
    if (strcmp(ast->args->args_arr[0], "echo") == 0) {
        if(redirect(ast)){
            if(is_bg)
                _exit(1);    
            return 1;
        }
        
        tmp_status = myth_echo(ast->args->args_arr);
        if(is_bg)
            _exit(tmp_status);    
        
        if(return_std(ast)){
            tmp_status = 1;
        }

        return tmp_status;
    } else if (strcmp(ast->args->args_arr[0], "exit") == 0) {
        
        tmp_status = myth_exit(ast->args->args_arr);
        if(tmp_status){
            if(is_bg)
                _exit(tmp_status);
            return tmp_status;
        }
        if(is_bg)
            _exit(tmp_status);
        free_ast(root);
        exit(tmp_status);

    } else if (strcmp(ast->args->args_arr[0], "cd") == 0) {
        if(redirect(ast)){
            if(is_bg)
                _exit(1);
            return 1;
        }
        
        tmp_status = myth_cd(ast->args->args_arr);
        
        if(return_std(ast)){
            tmp_status = 1;
        }
        if(is_bg)
            _exit(tmp_status);
        return tmp_status;
    }else if (strcmp(ast->args->args_arr[0], "pwd") == 0){
        if(redirect(ast)){
            if(is_bg)
                _exit(1);
            return 1;
        }
        tmp_status = myth_pwd(ast->args->args_arr);
        
        if(return_std(ast)){
            tmp_status = 1;
        }
        if(is_bg)
            _exit(tmp_status);
        return tmp_status;
    }else if (strcmp(ast->args->args_arr[0], "help") == 0){
        if(redirect(ast)){
            if(is_bg)
                _exit(1);
            return 1;
        }
        if(return_std(ast)){
            tmp_status = 1;
        }
        if(is_bg)
            _exit(tmp_status);
        return tmp_status;
    }else if (strcmp(ast->args->args_arr[0], "jobs") == 0){
        if(redirect(ast)){
            if(is_bg)
                _exit(1);
            return 1;
        }
        if(return_std(ast)){
            tmp_status = 1;
        }
        if(is_bg)
            _exit(tmp_status);
        return tmp_status;
    }else if (strcmp(ast->args->args_arr[0], "kill") == 0){
        if(redirect(ast)){
            if(is_bg)
                _exit(1);
            return 1;
        }
        if(return_std(ast)){
            tmp_status = 1;
        }
        if(is_bg)
            _exit(tmp_status);
        return tmp_status;
    }else if (strcmp(ast->args->args_arr[0], "fg") == 0){
        if(!config.interactive){
            if(is_bg)
                _exit(1);
            return 1;
        }
        if(redirect(ast)){
            if(is_bg)
                _exit(1);
            return 1;
        }
        if(return_std(ast)){
            tmp_status = 1;
        }
        if(is_bg)
            _exit(tmp_status);
        return tmp_status;

    }else if (strcmp(ast->args->args_arr[0], "bg") == 0){
        if(!config.interactive){
            if(is_bg)
                _exit(1);
            return 1;
        }
        if(redirect(ast)){
            if(is_bg)
                _exit(1);
            return 1;
        }
        if(return_std(ast)){
            tmp_status = 1;
        }
        if(is_bg)
            _exit(tmp_status);
        return tmp_status;
    }else if (strcmp(ast->args->args_arr[0], "wait") == 0){
        if(redirect(ast)){
            if(is_bg)
                _exit(1);
            return 1;
        }
        if(return_std(ast)){
            tmp_status = 1;
        }
        if(is_bg)
            _exit(tmp_status);
        return tmp_status;
    }else{
        if(!is_bg)
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
                config.last_status = errno;
                fprintf(stderr,"mysh: executor: execvp error %s\n",strerror(errno));
                _exit(errno);
           }
        }
        if(!is_bg)
            tmp_status = decode_wait(pr_pid);

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
                if(dup2(fd ,STDIN_FILENO) < 0){
                    config.last_status = errno;
                    // config.shell_status = errno;
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
                if(dup2(fd ,STDOUT_FILENO)<0){
                    config.last_status = errno;
                    // config.shell_status = errno;
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
                if(dup2(fd ,STDOUT_FILENO) < 0){
                    config.last_status = errno;
                    // config.shell_status = errno;
                    fprintf(stderr,"mysh: %s: %s\n",ast->redir->head[i].filename,strerror(errno));
                    close(fd);
                    return 1;
                }
                break;
            default:
                config.last_status = errno;
                // config.shell_status = errno;
                fprintf(stderr,"mysh: redirection error: %s\n",strerror(errno));
                return 1;  
        }
        close(fd);
    }   
    return 0;

}
int execute_pipeline(ASTNode *ast, bool is_bg){
    if(ast == NULL)
        return 1;
    int n = 0, size = SIZE_OF_PIPES;
    ASTNode* tmp_node = ast;
    int prev_fd = -1;
    int pipe_fd[2];

    ASTNode** stages = malloc(size * sizeof(ASTNode*));
    stages[n++] = ast->right;
    while(tmp_node->type == NODE_PIPELINE ){
        if(n >= size - 1){
            size *=2;
            stages = resize_node_array(stages ,size);
        }
        tmp_node = tmp_node->left;
        stages[n++] = tmp_node;
    }
    pid_t pids[n];
    for(int i = 0;i < n;i++){
        if (i < n - 1) {
            if(pipe(pipe_fd) < 0){
                config.last_status = errno;
                fprintf(stderr,"mysh: executor: pipe error %s\n",strerror(errno));
                return 1;    
            }
        }
        pids[i] = fork();
        if(pids[i] < 0 ){
            config.last_status = errno;
            fprintf(stderr,"mysh: executor: fork error %s\n",strerror(errno));
            return 1;
        }else if(pids[i] == 0){
            if(prev_fd != -1){ // проверяем на первый 
                if(dup2(prev_fd ,STDIN_FILENO) < 0){
                    config.last_status = errno;
                    // config.shell_status = errno;
                    fprintf(stderr,"mysh: %s: %s\n",ast->redir->head[n].filename,strerror(errno));
                    close(prev_fd);
                    return 1;
                }
                close(prev_fd);
            }
            if(i < n - 1){ // проверяем на последний  
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
            }
            
        }else{ // родитель
            if (prev_fd != -1) {
                close(prev_fd);
            }
    
            // Сохраняем чтение из текущего канала и закрываем запись
            if (i < n - 1) {
                close(pipe_fd[1]);
                prev_fd = pipe_fd[0];
            }

        }
    }



    free(stages);
    return 0;
}

int execute_subshell(ASTNode *ast, bool is_bg){
    if(ast == NULL)
        return 1;
    pr_pid = fork();
    if(pr_pid < 0 ){
        //config.shell_status = errno;
        fprintf(stderr,"mysh: executor: fork error %s\n",strerror(errno));
        return 1;
    }else if(pr_pid == 0){
        if(redirect(ast) != 0){
            _exit(1);    
        }
        tmp_status = execute_node(ast->left, false);
        _exit(tmp_status);
        }
    tmp_status = decode_wait(pr_pid);
    return tmp_status;

}
int return_std(ASTNode* ast){
    int result;
    if(ast->redir){
        result = dup2(saved_stdin, STDIN_FILENO);
        while(errno == EINTR && result < 0){
            result = dup2(saved_stdin, STDIN_FILENO);
        }
        if(result < 0){    
            config.shell_status = 3;
            fprintf(stderr,"mysh: dup2: %s\n",strerror(errno));
            close(saved_stdin);
            close(saved_stdout);

            return 1;
        }
        result = dup2(saved_stdout, STDOUT_FILENO);
        while(errno == EINTR && result < 0){
            result = dup2(saved_stdout, STDOUT_FILENO);
        }
        if(result < 0){
            config.shell_status = 3;
            fprintf(stderr,"mysh: dup2: %s\n",strerror(errno));
            close(saved_stdout);
            close(saved_stdin);
            return 1;
        }
    }
    return 0;
}
int decode_wait(pid_t pid){
    int raw_status = 0;
    int tmp = waitpid(pid,&raw_status,0);
    while(tmp < 0 && errno == EINTR){
        tmp = waitpid(pid,&raw_status,0);
    }
    if(tmp < 1)
        return 1;
     if (WIFEXITED(raw_status)) {
        return WEXITSTATUS(raw_status);
    } else if (WIFSIGNALED(raw_status)) {
        return 128 + WTERMSIG(raw_status);
    } else if (WIFSTOPPED(raw_status)) {
        return 128 + WSTOPSIG(raw_status);
    }
    return 1;
}

