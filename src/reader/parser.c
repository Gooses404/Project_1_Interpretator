#include "parser.h"
ASTNode* parse_tokens(TokenArray *tokens){
    parse_list(tokens);
}
ASTNode* parse_list(TokenArray *tokens){
    parse_and_or(tokens);
}
ASTNode* parse_and_or(TokenArray *tokens) { 

} 
ASTNode* parse_pipe(TokenArray *tokens){

} 
ASTNode* parse_command(TokenArray *tokens) {

}
RedirNode* parse_redirections(TokenArray *tokens, int *index){

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