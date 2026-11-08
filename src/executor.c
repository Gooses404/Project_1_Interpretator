#include "executor.h"
int read_bfs(ASTNode *ast){

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
