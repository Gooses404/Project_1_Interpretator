#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_WORD,       
    TOKEN_PIPE,         
    TOKEN_BG,           
    TOKEN_SEQ,          
    TOKEN_AND,          
    TOKEN_OR,           
    TOKEN_IN,           
    TOKEN_OUT,          
    TOKEN_APPEND,       
    // и так далее...   
} TokenType;


typedef enum{
    NORMAL,
    IN_SINGLE_QUOTE,
    IN_DOUBLE_QUOTE,
    IN_BACKSLASH,
}StateType;
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
typedef struct Token {
    TokenType type;
    char *value;           // Строка с текстом (имеет смысл только для TOKEN_WORD, для остальных можно NULL)
    struct Token *next;
    struct Token *prev;
} Token;
char* read_input();

void print_tokens(Token *head);
Token* tokenize(const char *text_of_input);
#endif
