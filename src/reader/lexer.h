#ifndef LEXER_H
#define LEXER_H
#define ARRAY_INITIAL_SIZE 16
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

} Token;

typedef struct TokenArray {
    Token *head;
    int cur_index;
    int cur_size;
}TokenArray; 

bool check_meta(unsigned char c);


TokenArray *CreateTokenArray();
void print_tokens(TokenArray *head);
TokenArray* AddToken(TokenArray *arr, TokenType type, const char *value, int value_length);
void destroy_tokens(TokenArray *list);


TokenArray* tokenize(char *text_of_input);
//char *tokenise_commment(char **current, char **start);

char *strncpy_no_quotes(const char *old_value, int old_value_lenght);

const char* GetTokenType(TokenType state);
const char *GetTokenValue(TokenType state);
#endif
