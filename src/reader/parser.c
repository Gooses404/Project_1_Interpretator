#include "parser.h"
int token_array_pos = 0; // 
int node_count = 0; // Отладка
ASTNode* parse_tokens(TokenArray *tokens){
    token_array_pos = 0; 
    ASTNode *root;    
    
    root = parse_list(tokens);
    token_array_pos++;
    
    return root;
}
ASTNode* parse_list(TokenArray *tokens){
    ASTNode *tmp ,*node = parse_and_or(tokens);
    if (!node) return NULL;
    while(token_array_pos < tokens->cur_index - 1 &&
         tokens->head[token_array_pos].type == TOKEN_SEQ){
            token_array_pos++;   
            tmp = create_node(NODE_SEQ);   
            ++node_count;
            tmp->left = node;
            tmp->right = parse_and_or(tokens);
            node = tmp;
    }
    
    return node;
}
ASTNode* parse_and_or(TokenArray *tokens) { 
    ASTNode *tmp ,*node = parse_pipe(tokens);
    if (!node) return NULL;
    while(token_array_pos < tokens->cur_index - 1 &&
        (tokens->head[token_array_pos].type == TOKEN_AND ||
        tokens->head[token_array_pos].type == TOKEN_OR)){
            switch (tokens->head[token_array_pos].type)
            {
            case TOKEN_AND:
                tmp = create_node(NODE_AND);
                ++node_count;
                token_array_pos++;
                tmp->left = node;
                tmp->right = parse_pipe(tokens);
                node = tmp;
                break;
            case TOKEN_OR:
                tmp = create_node(NODE_OR);
                ++node_count;
                token_array_pos++;
                tmp->left = node;
                tmp->right = parse_pipe(tokens);
                node = tmp;
                break;
            default:
                break;
            }
        
    }
    
    return node;
}


ASTNode* parse_pipe(TokenArray *tokens){
    ASTNode *tmp , *node = parse_simple(tokens);
    if (!node) return NULL;
        while(token_array_pos < tokens->cur_index - 1 &&
        tokens->head[token_array_pos].type == TOKEN_PIPE){
            tmp = create_node(NODE_PIPELINE);
            ++node_count;
            token_array_pos++;
            tmp->left = node;
            tmp->right = parse_simple(tokens);
            node = tmp;
        
    }
    return node;
}


/* Refir - нода с типом command 
но мы различаем команду с redirlist и просто redirlist только по количеству аргументов
*/
ASTNode* parse_simple(TokenArray *tokens){
    ASTNode *node ;
    if (tokens->head[token_array_pos].type == TOKEN_WORD ||
        is_redir(tokens->head[token_array_pos].type)){

        node = create_node(NODE_COMMAND);
        ++node_count;
        
        if(tokens->head[token_array_pos].type == TOKEN_WORD){
            node->args_count = 1;
            node->args = malloc(sizeof(char*) * ARG_ARRAY_INITIAL_SIZE); // первый аргумент это сама команда а далее 
            node->args[0] = strdup(tokens->head[token_array_pos].value);
            token_array_pos++;
        }
        
        while ( token_array_pos < tokens->cur_index &&(  //заполняем арг в цикле пока не встретим не слово или конец массива токенов
            tokens->head[token_array_pos].type == TOKEN_WORD||
            is_redir(tokens->head[token_array_pos].type))) {

            switch (tokens->head[token_array_pos].type)
                {
                case TOKEN_REDIRECT_IN:
                    if(tokens->head[++token_array_pos].type != TOKEN_WORD)
                    {
                        write(STDERR_FILENO,"2",1);
                        return NULL;
                    }

                    node->redir = add_redir_node(node->redir,TOKEN_REDIRECT_IN,tokens->head[token_array_pos].value);
                    ++token_array_pos;
                    break;
                case TOKEN_REDIRECT_OUT:
                    if(tokens->head[++token_array_pos].type != TOKEN_WORD)
                    {
                        write(STDERR_FILENO,"2",1);
                        return NULL;
                    }

                    node->redir = add_redir_node(node->redir,TOKEN_REDIRECT_OUT,tokens->head[token_array_pos].value);
                    ++token_array_pos;
                    break;
                case TOKEN_REDIRECT_D_OUT:
                    if(tokens->head[++token_array_pos].type != TOKEN_WORD)
                    {
                        write(STDERR_FILENO,"2",1);
                        return NULL;
                    }

                    node->redir = add_redir_node(node->redir,TOKEN_REDIRECT_D_OUT,tokens->head[token_array_pos].value);
                    ++token_array_pos; 

                    break;
                case TOKEN_WORD:
                    if(node->args == NULL){
                        node->args_count = 1;
                        node->args = malloc(sizeof(char*) * ARG_ARRAY_INITIAL_SIZE); // первый аргумент это сама команда а далее 
                        node->args[0] = strdup(tokens->head[token_array_pos].value);
                    }
                    if (node->args_count >= ARG_ARRAY_INITIAL_SIZE) {
                        node->args = realloc(node->args, sizeof(char*) * (node->args_count + 1));
                    }
                    
                    node->args[node->args_count] = strdup(tokens->head[token_array_pos].value);
                    node->args_count++;
                    token_array_pos++;
                default:
                    break;
            }
        }
        if(node->args != NULL){
            node->args[node->args_count] = NULL;
        }
        return node;


    }else if(tokens->head[token_array_pos].type == TOKEN_OPEN_BRACKET){
        token_array_pos++; // Пропуск (
        while(token_array_pos < tokens->cur_index - 1 && tokens->head[token_array_pos].type != TOKEN_CLOSE_BRACKET){
            node = create_node(NODE_SUBSHELL);
            ++node_count;
            node->left = parse_list(tokens); // subshell идет в left
         }                                  // Рекурсивно парсим содержимое скобок
        if (tokens->head[token_array_pos].type == TOKEN_CLOSE_BRACKET) {
            token_array_pos++;     // Пропуск )
        } else {
            // Обработка ошибки: ожидается закрывающая скобка
            fprintf(stderr, "Error: Expected closing bracket\n");
            free_ast(node);
            return NULL;
        }
        return node;
    }
    
    return NULL;
}
/*
ASTNode* parse_redirections(TokenArray *tokens){
    ASTNode *node;
    RedirArray *arr = create_redir_array(); 
    node = create_node(NODE_REDIR_LIST);
    
    while(token_array_pos < tokens->cur_index ){
            switch (tokens->head[token_array_pos].type)
                {
                case TOKEN_REDIRECT_IN:
                    if(tokens->head[++token_array_pos].type != TOKEN_WORD)
                    {
                        write(STDERR_FILENO,"2",1);
                        return NULL;
                    }

                    node->redir = add_redir_node(arr,TOKEN_REDIRECT_IN,tokens->head[token_array_pos].value);
                    ++token_array_pos;
                    break;
                case TOKEN_REDIRECT_OUT:
                    if(tokens->head[++token_array_pos].type != TOKEN_WORD)
                    {
                        write(STDERR_FILENO,"2",1);
                        return NULL;
                    }

                    node->redir = add_redir_node(arr,TOKEN_REDIRECT_OUT,tokens->head[token_array_pos].value);
                    ++token_array_pos;
                    break;
                case TOKEN_REDIRECT_D_OUT:
                    if(tokens->head[++token_array_pos].type != TOKEN_WORD)
                    {
                        write(STDERR_FILENO,"2",1);
                        return NULL;
                    }

                    node->redir = add_redir_node(arr,TOKEN_REDIRECT_D_OUT,tokens->head[token_array_pos].value);
                    ++token_array_pos; 

                    break;
                default:
                    goto redir_end;                
                    break;
                }
            }    
    

    redir_end:
    if (!node) return NULL;

    return node;
}


ASTNode* merge_redirections(ASTNode* node_1, ASTNode* node_2){
    

    return node_1;
}
*/
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
    node->args_count = 0;   
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
                for (int i = 0; i < node->args_count; i++) {
                    printf("%s ", node->args[i]);
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
            printf("SUBSHELL\n");
            break;
    }

    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
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
void free_ast(ASTNode *node){
    if (node == NULL) {
        return;
    }

    free_ast(node->left);
    free_ast(node->right);


    if (node->args != NULL) {
        for (int i = 0; i < node->args_count; i++) {
            free(node->args[i]);
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
    destroy_tokens(head);
}