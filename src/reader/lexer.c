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
    ['>']  = true,
    //////////
   // ['#']  = true
};

bool check_meta(unsigned char c) {
    return is_meta[c]; 
}
char* read_input(){
    printf("Start of reading\n");
    int bytes_read = 0 ,                size_of_text_of_input = 100;
    char *text_of_input = malloc(size_of_text_of_input * sizeof(char)),          
        symbol[1];
        
        
        while(1){    // loop for reading one symbol
       
        read(0, symbol, 1);
        if(symbol[0] == '\n'||symbol[0] == EOF || symbol[0] =='\0'){ 
            // printf("End of input text %s\n", text_of_input);
            break;
        }
        text_of_input[bytes_read] = symbol[0];
        //strncpy(text_of_input + bytes_read, symbol, 1 );
        
        ++bytes_read;
        if(bytes_read >= size_of_text_of_input - 1){
            size_of_text_of_input *= 2;
            text_of_input = Resize_char(text_of_input, size_of_text_of_input);
        }      
        ////printf(" %c",symbol[0]);    
    }
    if(bytes_read + 1  >= size_of_text_of_input - 1){
            size_of_text_of_input *= 2;
            text_of_input = Resize_char(text_of_input, size_of_text_of_input);
        }
    text_of_input[bytes_read] = '\0';
   //printf("End_of_reading %s ||a %d\n",text_of_input, strlen(text_of_input));
    return text_of_input ;

}

Token* tokenize(const char *text_of_input){
    //printf("Tokenize start\n");
    char *current = text_of_input, *start = text_of_input;
    Token *head = NULL;
    StateType state = NORMAL;
    StateType normal_state = NORMAL;
    int val_len = 0;
    while(*current != '\0'){
        
        if (*current == '#' && state == NORMAL ){
            state = IN_COMMENT;
            start = current;
        }

        if(state == IN_COMMENT ){
            
            if(*current == '\n' || *(current + 1) == '\0'){
                state = NORMAL;
                head = push_token_from_head(head, TOKEN_COMMENT, start, current - start +1);
            }
            ++current;
            continue;
        }

        if(state == NORMAL){
            if(*current == '\''){
                normal_state = IN_SINGLE_QUOTE;
                state = IN_SINGLE_QUOTE;
            }
             else if(*current == '"'){
                 
                 normal_state = IN_DOUBLE_QUOTE;
                 state = IN_DOUBLE_QUOTE;

            }else if(*current == '\\'){
                state = IN_BACKSLASH;
            }
            
        }  else if(*current == '\\' && state != IN_SINGLE_QUOTE){
                state = IN_BACKSLASH;
        } else if(state == IN_SINGLE_QUOTE ){
            if(*current == '\''){
                state = NORMAL;
                normal_state = NORMAL;
                
            }   
        } else if(state == IN_DOUBLE_QUOTE){
            if(*current == '\"'){
                state = NORMAL;
                normal_state = NORMAL;

            }
        } else if(state == IN_BACKSLASH){
            state = normal_state;
            if(*(current +1) == '\0'){
                head = push_token_from_head(head, TOKEN_WORD, start, current - start +1);
                ++current;
                continue;
            }else{
                ++current;

            }
        }

        
        if(check_meta(*current) || *(current + 1 ) =='\0' ||*(current + 1 ) =='#') // Должны запушить что нибудь               //*current ==' ' || *current == '\t' || *current == '\n' 
            {  
            if((*(current + 1 ) =='\0' || *(current + 1 ) =='#') && !check_meta(*current)){
                if( (check_meta(*start)) && start < current  ){ /// Ставаит start на начало след слова или спец символа, если start был пробелом, а current уже не пробел
                    start = current;
                }
            }
            
            if(state != NORMAL){
                ++current;
                continue;
            }
            /// WORD
            if(!check_meta(*start)){ //Пушим слово
                val_len = current - start;
                if((*(current + 1 ) =='\0' || *(current + 1 ) =='#' )&& !check_meta(*current)){
                    ++val_len; 
                }
                head = push_token_from_head(head, TOKEN_WORD, start, val_len);
                printf("STATE %d\n",state);

                start = current;
                
                
            }   

            switch (*current){
                case '|':
                    start = current;
                    if(*(current + 1 ) == '|'){
                        val_len = 2;
                        head = push_token_from_head(head, TOKEN_OR, start, val_len);
                        ++current;

                    }else if (*(current + 1 ) == '&'){
                        
                        val_len = 2;
                        head = push_token_from_head(head, TOKEN_ERROR_PIPE, start, val_len);
                        ++current;
                    }
                     else {
                        val_len =1;
                        head = push_token_from_head(head, TOKEN_PIPE, start, val_len);

                    }
    
                    break;
                case'&':
                start = current;
                 if(*(current + 1) == '&'){
                        val_len = 2;
                        head = push_token_from_head(head, TOKEN_AND, start, val_len);
                        ++current;
                    } else {
                        val_len =1;

                        head = push_token_from_head(head, TOKEN_BG, start, val_len);
                    }
                    break;
                    // не входят в базу: перенаправления с явным номером файлового дескриптора
                    // (`2> файл`, `2>&1`, `n>&-`),    
                case'<':
                    start = current;
                    val_len =1;
                    head = push_token_from_head(head, TOKEN_REDIRECT_IN, start, val_len);     
                    break;
                case'>':
                    start = current;
                    if(*(current + 1) == '>'){
                        val_len = 2;
                        head = push_token_from_head(head,TOKEN_REDIRECT_D_OUT , start, val_len);
                        ++current;
                    } else {
                        val_len =1;
                        head = push_token_from_head(head, TOKEN_REDIRECT_OUT, start, val_len);
                    }
                    break;
                case';':
                        start = current;
                        val_len =1;
                        head = push_token_from_head(head, TOKEN_SEQ, start, val_len);
                    break;
                case'(':
                    start = current;
                    val_len =1;
                    head = push_token_from_head(head, TOKEN_OPEN_BRACKET, start, val_len);
                    break;
                case')':
                    start = current;
                    val_len =1;
                    head = push_token_from_head(head, TOKEN_CLOSE_BRACKET, start, val_len);
                    break;
                default:
                    break;
                }
            
                start = current ;
               
        }
        if( (check_meta(*start)) && start < current ){ /// Ставаит start на начало след слова или спец символа, если start был спецсивмолом, а current уже не спецсимвол
            start = current;
        }
        
        
        ++current;
    }
    if(state != NORMAL){
        destroy_tokens(head);
        write(stderr,2,1);
        return NULL;
        
    }
    return head;
}
void print_tokens(Token *head){
   // printf("Start of printing tokns \n");
    if(head == NULL){
        printf("NULL head \n");

    }
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
       // printf("head ISNT NULL now \n");
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
void destroy_tokens(Token *head){
    Token *temp = head; 
    while(head != NULL){
        temp = head->next;

        free(head->value);
        free(head);
        head = temp;
    }
}




