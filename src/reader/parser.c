#include "parser.h"
#include "ast.h"
#include "commands.h"
int token_array_pos = 0; // 
int node_count = 0; // Отладка
unsigned int parser_errno = 0; // ошибка
bool parser_continue_state = 0; // продолжение
ASTNode* parse_tokens(TokenArray *tokens){
    if(tokens == NULL){
        return NULL;
    }
    ASTNode *root;       
    token_array_pos = 0;
    parser_errno = 0 ;

    root = parse_list(tokens);
    
    if(parser_errno != 0 && parser_errno != CONTINUE_ERROR){
        if(parser_errno & EMPTY_BRACKET_ERROR){
            write(STDERR_FILENO,"mysh: parser: empty_bracket\n",strlen("mysh: parser: empty_bracket\n"));
        }
        if(parser_errno & REDIR_ERROR){
            write(STDERR_FILENO,"mysh: parser: redir_error\n",strlen("mysh: parser: redir_error\n"));
        }
        if(parser_errno & UNCLOSED_BRACKET_ERROR){
            write(STDERR_FILENO,"mysh: parser: unclosed_bracket\n",strlen("mysh: parser: unclosed_bracket\n"));
        }
        if(parser_errno & UNEXPECTED_EOF_ERROR){
            write(STDERR_FILENO,"mysh: parser: unexpected_EOF\n",strlen("mysh: parser: unexpected_EOF\n"));
        }
            errno = 2;
            free_ast(root);
            config.last_status = 2;
            return NULL;
            
    }
    //printf("POS:%d SIZE:%d\n",token_array_pos,tokens->cur_index);
    //Доводим каретку если это \n
    if(token_array_pos < tokens->cur_index && tokens->head[token_array_pos].type == TOKEN_NEWLINE)
        ++token_array_pos;
    //Поскольку только при ошибке он будет отличатся от реального количеста элементов в массиве
    if(token_array_pos != tokens->cur_index){
        fprintf(stderr,"mysh: parser: unexpected_token %s\n",GetTokenValue(tokens->head[token_array_pos].type));
        errno = 2;
        config.last_status = 2;
        free_ast(root);
        return NULL;
    }
    // Отлавливаем ошибку , выводим сообщение и очищеам дерево   
    
    if(parser_errno & CONTINUE_ERROR){
            tokens = lexer_restart(tokens);
            free_ast(root);
            root = parse_tokens(tokens);
            if(root == NULL){
                node_count = 0;
                return NULL;
            }
        }

    return root;
}
ASTNode* parse_list(TokenArray *tokens){
    ASTNode *tmp ,*node = parse_and_or(tokens);
    if (!node) return NULL;
    while(token_array_pos < tokens->cur_index  &&
         (tokens->head[token_array_pos].type == TOKEN_SEQ ||
          tokens->head[token_array_pos].type == TOKEN_BG)){
            
            if(tokens->head[token_array_pos].type == TOKEN_SEQ){
                tmp = create_node(NODE_SEQ);
            }else{
                tmp = create_node(NODE_BG);
            }
            token_array_pos++;   
            ++node_count;
            tmp->left = node;
            tmp->right = parse_and_or(tokens);
            if(parser_errno != 0){
                free_ast(tmp);
                return NULL;
            }
            node = tmp;
            if(tmp->right == NULL){   
                break;
            }
    }
    
    return node;
}
ASTNode* parse_and_or(TokenArray *tokens) { 
    ASTNode *tmp ,*node = parse_pipe(tokens);
    if (!node) return NULL;
    while(token_array_pos < tokens->cur_index &&
        ( tokens->head[token_array_pos].type == TOKEN_AND ||
          tokens->head[token_array_pos].type == TOKEN_OR)){

            if(tokens->head[token_array_pos].type == TOKEN_AND){
                tmp = create_node(NODE_AND);
            }else{
                tmp = create_node(NODE_OR);
            }
            ++node_count;
            token_array_pos++;
            tmp->left = node;
            tmp->right = parse_pipe(tokens);
            if(tmp->right == NULL){
                if (parser_errno == 0 && !config.command_str && token_array_pos < tokens->cur_index && tokens->head[token_array_pos].type == TOKEN_NEWLINE) {
                    parser_errno |= CONTINUE_ERROR;
                }
                //Если через -с или в конце стоит не \n (файл кончился) то ставим флаг 
                else if(parser_errno == 0 && token_array_pos >= tokens->cur_index )
                    parser_errno |= UNEXPECTED_EOF_ERROR;

                //free_ast(node);
                free_ast(tmp);
                return NULL;
            }
            node = tmp;
        
    }
    
    return node;
}


ASTNode* parse_pipe(TokenArray *tokens){
    ASTNode *tmp , *node = parse_simple(tokens);
    if (!node) return NULL;
        while(token_array_pos < tokens->cur_index &&
        tokens->head[token_array_pos].type == TOKEN_PIPE){
            tmp = create_node(NODE_PIPELINE);
            ++node_count;
            token_array_pos++;
            tmp->left = node;
            tmp->right = parse_simple(tokens);
            if(tmp->right == NULL){
               if (parser_errno == 0 && !config.command_str && token_array_pos < tokens->cur_index && tokens->head[token_array_pos].type == TOKEN_NEWLINE) {
                    parser_errno |= CONTINUE_ERROR;
                }
                //Если через -с или в конце стоит не \n (файл кончился) то ставим флаг 
                else if(parser_errno == 0 && token_array_pos >= tokens->cur_index )
                    parser_errno |= UNEXPECTED_EOF_ERROR;

                //free_ast(node);
                free_ast(tmp);
                return NULL;
            }
            node = tmp;
        
    }
    return node;
}


/* Redir - нода с типом command 
но мы различаем команду с redirlist и просто redirlist только по количеству аргументов
*/
ASTNode* parse_simple(TokenArray *tokens){
    if(token_array_pos >= tokens->cur_index){
        return NULL;
    }
    ASTNode *node ;
    RedirArray *tmp_redir = NULL;
    if (tokens->head[token_array_pos].type == TOKEN_WORD ||
        is_redir(tokens->head[token_array_pos].type)){

        node = create_node(NODE_COMMAND);
        ++node_count;
        //заполняем арг в цикле пока не встретим не слово , redir или конец массива токенов
        //
        while ( token_array_pos < tokens->cur_index   
            &&( tokens->head[token_array_pos].type == TOKEN_WORD||
                is_redir(tokens->head[token_array_pos].type))) {

            if(tokens->head[token_array_pos].type == TOKEN_WORD){  //если слово
                if(node->args == NULL){ 
                        node->args = create_args_array();          // создаем обертку если ее нет 
                    }
                if (node->args->args_count >= node->args->args_size - 1) {
                    node->args->args_size *= 2;
                    node->args->args_arr = resize_args_array(node->args->args_arr, node->args->args_size);
                }
                node->args->args_arr[node->args->args_count] = strdup(tokens->head[token_array_pos].value);
                node->args->args_count++;
            }else{
                tmp_redir = parse_single_redirection(node->redir,tokens);
                if(tmp_redir == NULL){
                    errno = 2;
                    parser_errno |= REDIR_ERROR;
                    free_ast(node);
                    return NULL;
                }
                node->redir = tmp_redir;
            }
            
            ++token_array_pos;
        }

        /// Ставим NULL в конец если массив сущ 
        if(node->args != NULL){
            node->args->args_arr[node->args->args_count] = NULL;
        }
        return node;


    }else if(tokens->head[token_array_pos].type == TOKEN_OPEN_BRACKET){
        token_array_pos++; // Пропуск (
        node = create_node(NODE_SUBSHELL);
        ++node_count;
        node->left = parse_list(tokens); // subshell идет в left
        if(token_array_pos < tokens->cur_index && tokens->head[token_array_pos].type == TOKEN_NEWLINE && !config.command_str){
            tokens->head[token_array_pos].type = TOKEN_SEQ;
            parser_errno |= CONTINUE_ERROR;
            return node;    
        }
        if(node->left == NULL){
            if(parser_errno == 0 ){
                errno = 2;
                parser_errno |= EMPTY_BRACKET_ERROR;
                token_array_pos++; // Чтобы не вызывался unexpected token
                free_ast(node);
                return NULL;
           }else{
                free_ast(node);
                return NULL;
           }
        }
       // token_array_pos++; 
       // printf("TOKEN NOW %s\n",GetTokenValue(tokens->head[token_array_pos].type));
       
       
       if(token_array_pos >= tokens->cur_index|| tokens->head[token_array_pos].type == TOKEN_NEWLINE || tokens->head[token_array_pos].type != TOKEN_CLOSE_BRACKET){
            errno = 2;
            parser_errno |= UNCLOSED_BRACKET_ERROR;
            free_ast(node);
            return NULL;
            
        }
        token_array_pos++; // Пропуск )
        // redir могут быть только после скобок
        while( token_array_pos < tokens->cur_index && is_redir(tokens->head[token_array_pos].type)){
            tmp_redir = parse_single_redirection(node->redir,tokens);
            if(tmp_redir == NULL){
                errno = 2;
                parser_errno |= REDIR_ERROR;
                free_ast(node);
                return NULL;
            }
            node->redir = tmp_redir;
            ++token_array_pos;

        }
        return node;
    }
    
    return NULL;
}
RedirArray* create_redir_array(){
    RedirArray *array = malloc(sizeof(RedirArray));
    array->head = malloc(sizeof(RedirNode) * R_ARRAY_INITIAL_SIZE); // Начальный размер массива
    array->cur_index = 0;
    array->cur_size = R_ARRAY_INITIAL_SIZE;
    return array;
}



ASTNode* create_node(ASTNodeType type){
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->args = NULL;
    node->redir = NULL;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}
/*
*/
RedirArray* add_redir_node(RedirArray* array, TokenType type, const char *filename){
    if(array == NULL){
        array = create_redir_array();
    }
    RedirNode *new_redir = &(array->head[array->cur_index++]) ; 

    new_redir->filename = malloc(strlen(filename) + 1); 
    strcpy(new_redir->filename, filename);
    new_redir->filename[strlen(filename)] = '\0';
    new_redir->type = type;
    if(array->cur_index + 1  >= array->cur_size){
        array->cur_size *= 2;
        array->head = resize_redir_array(array->head, array->cur_size);
    }
    
    return array;
}
RedirArray *parse_single_redirection(RedirArray *redir,TokenArray *tokens){
    if(token_array_pos + 1 >= tokens->cur_index 
    ||  tokens->head[token_array_pos + 1].type != TOKEN_WORD){
            return NULL;
        }
    
    switch (tokens->head[token_array_pos].type){
        case TOKEN_REDIRECT_IN:
            return add_redir_node(redir,TOKEN_REDIRECT_IN,tokens->head[++token_array_pos].value);
            break;
        case TOKEN_REDIRECT_OUT:
            return add_redir_node(redir,TOKEN_REDIRECT_OUT,tokens->head[++token_array_pos].value);
            break;
        case TOKEN_REDIRECT_D_OUT:
            return add_redir_node(redir,TOKEN_REDIRECT_D_OUT,tokens->head[++token_array_pos].value);
            break;
        default:
            break;
    }
    return NULL;
}


bool is_redir(TokenType type){
    return( type == TOKEN_REDIRECT_IN ||
            type == TOKEN_REDIRECT_OUT ||     
            type == TOKEN_REDIRECT_D_OUT);
}
RedirNode* resize_redir_array(RedirNode* redir, int size){
    RedirNode* new_arr = (RedirNode*)realloc(redir, size * sizeof(RedirNode));
    if (new_arr == NULL) {    
        free(redir);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_arr;

}
ArgsArray* create_args_array(){
    ArgsArray* array = (ArgsArray*)malloc(sizeof(ArgsArray));;
    array->args_arr = (char**)malloc(sizeof(char**) * ARRAY_INITIAL_SIZE); // Начальный размер массива
    array->args_count = 0;
    array->args_size = ARRAY_INITIAL_SIZE;
    return array;
}


char** resize_args_array(char** args, int size){
    char** new_arr = (char**)realloc(args, size * sizeof(char**));
    if (new_arr == NULL) {    
        free(args);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_arr;

}
void print_ast(ASTNode *node, int level) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    switch (node->type) {
        case NODE_COMMAND:
            printf("COMMAND: ");
            if(node->args != NULL){
                if(node->args->args_arr != NULL){
                    for (int i = 0; i < node->args->args_count; i++) {
                        printf("%s ", node->args->args_arr[i]);
                    }
                }
            }
            if(node->redir == NULL){
                printf("\n");
                break;
            }
            for (int i = 0; i < node->redir->cur_index; i++) {
                printf("redir %d: %s ",node->redir->head[i].type, node->redir->head[i].filename);
            }
            
            printf("\n");
            break;
        case NODE_PIPELINE:
            printf("PIPELINE\n");
            break;
        case NODE_AND:
            printf("AND\n");
            break;
        case NODE_OR:
            printf("OR\n");
            break;
        case NODE_SEQ:
            printf("SEQ\n");
            break;
        case NODE_BG:
            printf("BG\n");
            break;
        case NODE_SUBSHELL:
            printf("SUBSHELL");
            if(node->redir == NULL){
                printf("\n");
                break;
            }
            printf(": ");

            for (int i = 0; i < node->redir->cur_index; i++) {
                printf("redir %d: %s ",node->redir->head[i].type, node->redir->head[i].filename);
            }
            printf("\n");
            
            break;
    }

    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

void free_ast(ASTNode *node){
    if (node == NULL) {
        return;
    }

    free_ast(node->left);
    free_ast(node->right);


    if (node->args != NULL) {
        if(node->args->args_arr != NULL){
            for (int i = 0; i < node->args->args_count; i++) {
                free(node->args->args_arr[i]);
            }
            free(node->args->args_arr);
        }
        free(node->args);
    }

    // if(node->subshell != NULL){
    //     free_ast(node->subshell);
    // }
    if (node->redir != NULL) {
        for (int i = 0; i < node->redir->cur_index; i++) {
            free(node->redir->head[i].filename);
        }
        free(node->redir->head);
        free(node->redir);
    }

    free(node);
}



void parser_test_mode(){
    TokenArray *head;
    ASTNode *root;
    head = lexer_test_mode();
    root = parse_tokens(head);
    print_ast(root, 0);
    printf("Node count: %d\n",node_count);
    node_count = 0;
    free_ast(root);
    free_tokens(head);
}