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

TokenList* tokenize(char *text_of_input){
    if(text_of_input == NULL){
        
    }
    //printf("Tokenize start\n");
    char *current = text_of_input, *start = text_of_input;
    TokenList * list = CreateTokenList(); 
    StateType state = NORMAL;
    StateType normal_state = NORMAL; //нужен чтобы после \ вернуться к правильному состоянию
    int val_len = 0;
    int is_skiped = 0;
    while(*current != '\0'){

        if (*current == '#' && state == NORMAL  && ( current == start || check_meta(*(current - 1)))){ // проверка что в начале слова
            state = IN_COMMENT;
            //printf("com start");
            // if(current != start){
            //     list = push_token_from_tail(list, TOKEN_COMMENT, start, current - start );
            // }
            start = current;
        }

        if(state == IN_COMMENT ){ // продолжаем комментарий до конца строки 
            
            if(*current == '\n' || *(current + 1) == '\0'){
                state = NORMAL;
                val_len = current - start - 1 ;
              //  list = push_token_from_tail(list, TOKEN_COMMENT, start + 1, val_len);
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
            if(*(current + 1) == '\0'){
                list = push_token_from_tail(list, TOKEN_WORD, start, current - start +1);
                is_skiped = 1;
                continue;
            }else{
                is_skiped = 1;
                

            }
        }
    if(!is_skiped)
    {    
        if(check_meta(*current) || *(current + 1 ) =='\0' ) // Должны запушить что нибудь               //*current ==' ' || *current == '\t' || *current == '\n' 
            {  
            if((*(current + 1 ) =='\0' ) && !check_meta(*current)){
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
                if((*(current + 1 ) =='\0'  )&& !check_meta(*current)){
                    ++val_len; 
                }
               
                list = push_token_from_tail(list, TOKEN_WORD, start, val_len);
               // printf("STATE %d\n",state);

                start = current;
                
                
            }   

            switch (*current){
                case '|':
                    start = current;
                    if(*(current + 1 ) == '|'){
                        val_len = 2;
                        list = push_token_from_tail(list, TOKEN_OR, start, val_len);
                        ++current;

                    }else if (*(current + 1 ) == '&'){
                        
                        val_len = 2;
                        list = push_token_from_tail(list, TOKEN_ERROR_PIPE, start, val_len);
                        ++current;
                    }
                     else {
                        val_len =1;
                        list = push_token_from_tail(list, TOKEN_PIPE, start, val_len);

                    }
    
                    break;
                case'&':
                start = current;
                 if(*(current + 1) == '&'){
                        val_len = 2;
                        list = push_token_from_tail(list, TOKEN_AND, start, val_len);
                        ++current;
                    } else {
                        val_len =1;

                        list = push_token_from_tail(list, TOKEN_BG, start, val_len);
                    }
                    break;
                    // не входят в базу: перенаправления с явным номером файлового дескриптора
                    // (`2> файл`, `2>&1`, `n>&-`),    
                case'<':
                    start = current;
                    val_len =1;
                    list = push_token_from_tail(list, TOKEN_REDIRECT_IN, start, val_len);     
                    break;
                case'>':
                    start = current;
                    if(*(current + 1) == '>'){
                        val_len = 2;
                        list = push_token_from_tail(list,TOKEN_REDIRECT_D_OUT , start, val_len);
                        ++current;
                    } else {
                        val_len =1;
                        list = push_token_from_tail(list, TOKEN_REDIRECT_OUT, start, val_len);
                    }
                    break;
                case';':
                        start = current;
                        val_len =1;
                        list = push_token_from_tail(list, TOKEN_SEQ, start, val_len);
                    break;
                case'(':
                    start = current;
                    val_len =1;
                    list = push_token_from_tail(list, TOKEN_OPEN_BRACKET, start, val_len);
                    break;
                case')':
                    start = current;
                    val_len =1;
                    list = push_token_from_tail(list, TOKEN_CLOSE_BRACKET, start, val_len);
                    break;
                case'\n':
                    start = current;
                    val_len =1;
                    list = push_token_from_tail(list, TOKEN_SEQ, start, val_len);
                    break;
                default:
                    break;
                }
            
                start = current ;
               
        }
        if( (check_meta(*start)) && start < current ){ /// Ставаит start на начало след слова или спец символа, если start был спецсивмолом, а current уже не спецсимвол
            start = current;
        }
    }
        
        ++current;
        is_skiped =0;
    }
    if(state != NORMAL){
        destroy_tokens(list);
        write(2,"2",1);
        return NULL;
        
    }
    return list;
}
void print_tokens(TokenList *list){
    
   // printf("Start of printing tokns \n");
    if (list == NULL || list->head == NULL) {
        printf("NULL head \n");
        return;
    }
   Token *tmp = list->head;
    if(tmp == NULL){
        printf("NULL head \n");

    }
    while(tmp != NULL){
        printf("Token type: %s, value: %s\n", GetTokenType(tmp->type), tmp->value);
        tmp = tmp->next;
    }
}
TokenList *CreateTokenList(){
    TokenList *list =malloc(sizeof(TokenList));
    list->head = NULL; 
    list->tail = NULL;
    return list; 
}
TokenList* push_token_from_head(TokenList *list, TokenType type, const char *value, int value_length){
    Token *new_token = malloc(sizeof(Token));        //  и нужно создать токен
    new_token->type = type;
    new_token->value = strncpy_no_quotes( value , value_length);
    new_token->next = NULL;
    new_token->prev = NULL;
    
    if(list->head == NULL){
        list->head = new_token;
        list->tail = new_token;
       // printf("head ISNT NULL now \n");
    } else {
        Token *temp = list->head;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = new_token;
        new_token->prev = temp;
        list->tail = new_token;

    }
    return list;
}
TokenList* push_token_from_tail(TokenList *list, TokenType type, const char *value, int value_length){
    Token *new_token = malloc(sizeof(Token));        //  и нужно создать токен
    new_token->type = type;
    if(type == TOKEN_WORD){
        new_token->value = strncpy_no_quotes( value , value_length);
    }else if(type == TOKEN_COMMENT){
        new_token->value = malloc(sizeof(char) * value_length + 1 );
        strncpy(new_token->value, value , value_length);
        new_token->value[value_length] = '\0';
    }else{
        new_token->value = NULL;
    }
    new_token->next = NULL;
    new_token->prev = NULL;
    
    if(list->tail == NULL){
        list->head = new_token;
        list->tail = new_token;
       // printf("head ISNT NULL now \n");
    } else {
        list->tail->next = new_token;
        new_token->prev = list->tail;
        list->tail = new_token;

    }
    return list;
}
void destroy_tokens(TokenList *list){
    if (list == NULL) {
        return;
    }
    Token *temp = list->head; 
    while(list->head != NULL){
        temp = list->head->next;

        free(list->head->value);
        free(list->head);
        list->head = temp;
    }
    list->tail = NULL;
    free(list);
}

const char *GetTokenType(TokenType state) {
    switch (state) {
        case TOKEN_WORD:            return "WORD";
        case TOKEN_COMMENT:         return "COMMENT";
        case TOKEN_PIPE:            return "PIPE";
        case TOKEN_ERROR_PIPE:      return "ERROR_PIPE";
        case TOKEN_BG:              return "BG";
        case TOKEN_SEQ:             return "SEQ";
        case TOKEN_AND:             return "AND";
        case TOKEN_OR:              return "OR";  
        case TOKEN_REDIRECT_IN:     return "REDIRECT_IN";
        case TOKEN_REDIRECT_D_OUT:  return "REDIRECT_D_OUT";
        case TOKEN_REDIRECT_OUT:    return "REDIRECT_OUT";
        case TOKEN_OPEN_BRACKET:    return "OPEN_BRACKET";
        case TOKEN_CLOSE_BRACKET:   return "CLOSE_BRACKET";
        default:                    return "UNKNOWN";
    }
}
// Копирование без кавычек и слэшей
char *strncpy_no_quotes( const char *old_value,  int old_value_length) {
    //printf("%d",old_value[0]);
    char *new_value;
    int is_backslash = 0;
    StateType state = NORMAL;
    new_value = malloc(sizeof(char)* old_value_length +1 );  //для \0
    
    char *new_ptr = new_value;
    const char *old_ptr = old_value;
    int written = 0;
    for (int i = 0; i < old_value_length; ++i, ++old_ptr) {
        if(!is_backslash)
        {if(*old_ptr == '\\'){
             switch(state){
                case NORMAL:
                    is_backslash = 1 ;
                    continue;
                case IN_DOUBLE_QUOTE:
                    if( i + 1 < old_value_length &&(*(old_ptr + 1) == '"'|| *(old_ptr + 1) == '\\' || *(old_ptr + 1) == '\n')){
                        is_backslash = 1 ;
                        continue;
                    }
                    break;  
                case IN_SINGLE_QUOTE:
                    break;
                    
            }

            
            }
        if ( *old_ptr == '"' ) {
            switch(state){
                case NORMAL:
                    state = IN_DOUBLE_QUOTE;
                    continue;
                case IN_DOUBLE_QUOTE:
                    state = NORMAL;
                    continue;
                case IN_SINGLE_QUOTE:
                    break;    
            }

        }else if(*old_ptr == '\''){
            switch(state){
                case NORMAL:
                    state = IN_SINGLE_QUOTE;
                    continue;
                case IN_SINGLE_QUOTE:
                    state = NORMAL;
                    continue;
                case IN_DOUBLE_QUOTE:
                    break;
            }
        }
        }

        *new_ptr = *old_ptr;
        ++new_ptr;
        ++written;
        is_backslash = 0;
    }
    new_value[written] = '\0';  //оставляем небольшой хвост который в любом случае уберется при помощи free 
    return new_value;
}

