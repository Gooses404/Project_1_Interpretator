#include "lexer.h"
#include "commands.h"
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
        } else if(state == IN_SINGLE_QUOTE){
            if(*current == '\''){
                state = NORMAL;
            }
        } else if(state == IN_DOUBLE_QUOTE){
            if(*current == '"'){
                state = NORMAL;
            }
        } else if(state == IN_BACKSLASH){
            state = NORMAL;
        }
        
        if(*current ==' ' || *current == '\t' || *current == '\n' || state == NORMAL){
            
            if(*start != ' '&& *start != '\t' && *start != '\n'){ /// Происходит когда мы дошли до конца слова ~ начало не пробел а каретка уже не буква или символ,
                                              
                Token *new_token = malloc(sizeof(Token));        //  и нужно создать токен
                new_token->type = TOKEN_WORD;
                new_token->value = malloc((current - start + 1) * sizeof(char));
                strncpy(new_token->value, start, current - start);
                new_token->value[current - start] = '\0';
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
            }
            
            
            
            start = current ;
            continue;
        }
        if( (*start == ' '|| *start == '\t' || *start == '\n') && *start != *current){

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


