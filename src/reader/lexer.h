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

typedef struct TokenList {
    Token *head;
    Token *tail;
}TokenList; 

bool check_meta(unsigned char c);


TokenList *CreateTokenList();
void print_tokens(TokenList *head);
TokenList* push_token_from_head(TokenList *list, TokenType type, const char *value, int value_length);
TokenList* push_token_from_tail(TokenList *list, TokenType type, const char *value, int value_length);
void destroy_tokens(TokenList *list);


TokenList* tokenize(char *text_of_input);
char *strncpy_no_quotes(const char *old_value, int old_value_lenght);

const char* GetTokenType(TokenType state);
const char *GetTokenValue(TokenType state);
#endif
