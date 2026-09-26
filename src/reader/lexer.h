#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_WORD,   
    TOKEN_COMMENT,    
    TOKEN_PIPE, 
    TOKEN_ERROR_PIPE,         
    TOKEN_BG,           
    TOKEN_SEQ,          
    TOKEN_AND,          
    TOKEN_OR,           
    TOKEN_IN,           
    TOKEN_OUT,          
    TOKEN_APPEND,       
    TOKEN_REDIRECT_IN,   
    TOKEN_REDIRECT_D_OUT,   
    TOKEN_REDIRECT_OUT, 
    TOKEN_OPEN_BRACKET,  
    TOKEN_CLOSE_BRACKET,  
} TokenType;


typedef enum{
    NORMAL,
    IN_SINGLE_QUOTE,
    IN_DOUBLE_QUOTE,
    IN_BACKSLASH,
    IN_COMMENT,
}StateType;
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
typedef struct Token {
    TokenType type;
    char *value;           // Строка с текстом (имеет смысл только для TOKEN_WORD, для остальных можно NULL)
    struct Token *next;
    struct Token *prev;
} Token;

bool check_meta(unsigned char c);

char* read_input();

void print_tokens(Token *head);
Token* push_token_from_head(Token *head, TokenType type, const char *value, int value_length);
void destroy_tokens(Token *head);


Token* tokenize(const char *text_of_input);
#endif
