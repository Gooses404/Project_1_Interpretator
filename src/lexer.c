#include "lexer.h"
#include "commands.h"
#include <stdbool.h>

static const bool is_meta[256] = {
    [' ']  = true,
    ['\t'] = true,
    ['\n'] = true,
    ['|']  = true,
    ['&']  = true,
    [';']  = true,
    ['(']  = true,
    [')']  = true,
    ['<']  = true,
    ['>']  = true
};

bool check_meta(unsigned char c) {
    return is_meta[c]; 
}
char* read_input(){
    int bytes_read = 0 ,                size_of_text_of_input = 100;
    char *text_of_input = malloc(size_of_text_of_input * sizeof(char)),          
        symbol[1];
        
        
        while(1){    // loop for reading one symbol
        read(0, symbol, 1);
        if(symbol[0] == '\n'||symbol[0] == EOF || symbol[0] =='\0'){ 
            // printf("End of input text %s\n", text_of_input);
            break;
        }
        strcpy(text_of_input + bytes_read, symbol);
        
        ++bytes_read;
        if(bytes_read >= size_of_text_of_input - 1){
            Resize_char(text_of_input, size_of_text_of_input);
        }      
        }
    bytes_read = 0;
    return text_of_input + '\0';

}

Token* tokenize(const char *text_of_input){
    char *current = text_of_input, *start = text_of_input;
    Token *head = NULL;
    StateType state = NORMAL;
    while(*current != '\0'){
        if (*current == '#'){
            state = IN_COMMENT;
            start = current;
        }
        
        if(state == IN_COMMENT){
            
            if(*current == '\n' || *
                ++current == '\0'){
                state = NORMAL;
                push_token_from_head(head, TOKEN_COMMENT, start, current - start);
            }
            ++current;
;
            continue;
            
        }

        if(state == NORMAL){
            if(*current == '\''){
                state = IN_SINGLE_QUOTE;
            }
             else if(*current == '"'){
                state = IN_DOUBLE_QUOTE;

            }
             else if(*current == '\\'){
                state = IN_BACKSLASH;
            } 
        } else if(state == IN_SINGLE_QUOTE ){
            if(*current == '\''){
                state = NORMAL;
            }
        } else if(state == IN_DOUBLE_QUOTE){
            if(*current == '\"'){
                state = NORMAL;
            }
        } else if(state == IN_BACKSLASH){
            state = NORMAL;
        }

        
        if(check_meta(*current) || *(++current) =='\0'                //*current ==' ' || *current == '\t' || *current == '\n' 
        ){
            if(state != NORMAL){
                ++current;
                continue;
            }
            switch (*current){
            case '|':
                if(*(++current) == '|'){
                    push_token_from_head(head, TOKEN_OR, start, current - start + 1);
                    ++current;
                } else {
                    push_token_from_head(head, TOKEN_PIPE, start, current - start);
                }

                break;
            case'&':
             if(*(++current) == '&'){
                    push_token_from_head(head, TOKEN_AND, start, current - start + 1);
                    ++current;
                } else {
                    push_token_from_head(head, TOKEN_PIPE, start, current - start);
                }
                break;
            default:
                break;
            }

            if(*start != ' '&& *start != '\t' && *start != '\n'){ /// Происходит когда мы дошли до конца слова ~ начало не пробел а каретка уже не буква или символ,
                // if((*start < 'z' && *start > 'a') || (*start < 'Z' && *start > 'A')){                       // если start это буква то мы добавляем токен слова в список
                    push_token_from_head(head, TOKEN_WORD, start, current - start);

                // }

                }   
               
            
            
            
          //  if(*current == ' '|| *current == '\t' || *current == '\n' && *++current!='\0'){
                start = current ;
           // }
            //++current;
            //continue;
        }
        if( (*start == ' '|| *start == '\t' || *start == '\n') && *start != *current){ /// Ставаит start на начало след слова или спец символа, если start был пробелом, а current уже не пробел
            start = current;
        }
        
        
        ++current;
    }
    return head;
}
void print_tokens(Token *head){
    while(head != NULL){
        printf("Token type: %d, value: %s\n", head->type, head->value);
        head = head->next;
    }
}
Token* push_token_from_head(Token *head, TokenType type, const char *value, int value_length){
    Token *new_token = malloc(sizeof(Token));        //  и нужно создать токен
    new_token->type = type;
    new_token->value = malloc(sizeof(char)* value_length + 1);           //((current - start + 1) * sizeof(char));
    strncpy(new_token->value, value, value_length);
    new_token->value[value_length] = '\0';
    new_token->next = NULL;
    new_token->prev = NULL;
    
    if(head == NULL){
        head = new_token;
    } else {
        Token *temp = head;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = new_token;
        new_token->prev = temp;
    }
    return head;
}



