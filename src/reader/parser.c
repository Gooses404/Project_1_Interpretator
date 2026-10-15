#include "parser.h"
ASTNode* parse_tokens(TokenArray *tokens){
    parse_list(tokens);
}
ASTNode* parse_list(TokenArray *tokens){
    parse_and_or(tokens);
}
ASTNode* parse_and_or(TokenArray *tokens) { 

} 
ASTNode* parse_pipe(TokenArray *tokens, ASTNode* cur_node){
    ASTNode *tmp;
    if(tokens->head[token_array_pos].type == TOKEN_PIPE){
            tmp = create_node(NODE_PIPELINE);
            tmp->left = cur_node;
            tmp->right = NULL;
            cur_node = tmp;
            token_array_pos++;
        
    }
    if(cur_node->type == NODE_PIPELINE){
        cur_node->right = parse_subshell(tokens, cur_node->right);
    }else{
        cur_node->left = parse_subshell(tokens, cur_node->left);
    }
    return cur_node;
}


ASTNode* parse_subshell(TokenArray *tokens, ASTNode* cur_node){
    ASTNode *tmp;
    if(tokens->head[token_array_pos].type == TOKEN_PIPE){
            tmp = create_node(NODE_PIPELINE);
            tmp->left = cur_node;
            tmp->right = NULL;
            cur_node = tmp;
            token_array_pos++;
        
    }
    if(cur_node->type == NODE_PIPELINE){
        cur_node->right = parse_command(tokens, cur_node->right);
    }else{
        cur_node->left = parse_command(tokens, cur_node->left);
    }
    return cur_node;

}

ASTNode* parse_command(TokenArray *tokens, ASTNode* cur_node){
    if (tokens->head[token_array_pos].type == TOKEN_WORD) {
        ASTNode *node = create_node(NODE_COMMAND);
        node->args_count = 1;
        node->args = malloc(sizeof(char*) * ARG_ARRAY_INITIAL_SIZE); // первый аргумент это сама команда а далее 
        node->args[0] = strdup(tokens->head[token_array_pos].value);
        token_array_pos++;
        
        while ( token_array_pos < tokens->cur_index &&  //заполняем арг в цикле пока не встретим не слово или конец массива токенов
            tokens->head[token_array_pos].type == TOKEN_WORD) {
            if (node->args_count >= ARG_ARRAY_INITIAL_SIZE) {
                node->args = realloc(node->args, sizeof(char*) * (node->args_count + 1));
            }
            node->args[node->args_count] = strdup(tokens->head[token_array_pos].value);
            node->args_count++;
            token_array_pos++;

        }
        
        return node;
    }else{
        parse_redirections(tokens);
        return NULL;
    }

}
RedirNode* parse_redirections(TokenArray *array){
    return NULL;
}



RedirArray* CreateRedirArray(){
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
    node->subshell_body = NULL;
    return node;
}
RedirArray* add_redir_node(RedirArray* array, TokenType type, const char *filename){
    if(array == NULL){
        array = CreateRedirArray();
    }
    RedirNode *new_redir = &(array->head[array->cur_index++]) ; 

    new_redir->filename = malloc(strlen(filename) + 1); 
    strcpy(new_redir->filename, filename);

    if(array->cur_index + 1  >= array->cur_size){
        array->cur_size *= 2;
        array->head = Resize_Redir_Array(array->head, array->cur_size);
    }
    
    return array;
}



RedirNode* Resize_Redir_Array(RedirNode* redir, int size){
    RedirNode* new_arr = (RedirNode*)realloc(redir, size * sizeof(RedirNode));
    if (new_arr == NULL) {    
        free(redir);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_arr;
}