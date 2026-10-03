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

TokenArray* tokenize(char *text_of_input){
    if(text_of_input == NULL){
        return NULL;
    }
    //printf("Tokenize start\n");
    char *current = text_of_input, *start = text_of_input;
    TokenArray * array = CreateTokenArray(); 
    StateType state = NORMAL;
    StateType normal_state = NORMAL; //нужен чтобы после \ вернуться к правильному состоянию
    int val_len = 0;
    int is_skiped = 0;
    while(*current != '\0'){

        if (*current == '#' && state == NORMAL  && ( current == start || check_meta(*(current - 1)))){ // проверка что в начале слова
            state = IN_COMMENT;
            //printf("com start");
            // if(current != start){
            //     list = AddToken(list, TOKEN_COMMENT, start, current - start );
            // }
            start = current;
        }

        if(state == IN_COMMENT ){ // продолжаем комментарий до конца строки 
            
            if(*current == '\n' || *(current + 1) == '\0'){
                state = NORMAL;

                //val_len = current - start - 1 ;
              //  list = AddToken(list, TOKEN_COMMENT, start + 1, val_len);
            if(*current == '\n'){
                start = current;
                val_len =1;
                array = AddToken(array, TOKEN_SEQ, start, val_len);

                }
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
        } else if(state == IN_BACKSLASH){
            state = normal_state;
            if(*(current + 1) == '\0'){
                array = AddToken(array, TOKEN_WORD, start, current - start +1);
                is_skiped = 1;
                continue;
            }else{
                is_skiped = 1;
                

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
        
        }
    if(!is_skiped)
    {   if( (check_meta(*start)) && start < current ){ /// Ставаит start на начало след слова или спец символа, если start был спецсивмолом, а current уже не спецсимвол
            start = current;
        } 
        if(check_meta(*current) || *(current + 1 ) =='\0' ) // Должны запушить что нибудь               //*current ==' ' || *current == '\t' || *current == '\n' 
            {  
            
            if(state != NORMAL){
                ++current;
                continue;
            }
            /// WORD
            if(!check_meta(*start)){ //Пушим слово
                val_len = current - start;
                if((*(current + 1 ) =='\0'  )&& !check_meta(*current)){ // проверка того что слово именно в конце строки
                    ++val_len; 
                }
               
                array = AddToken(array, TOKEN_WORD, start, val_len);
               

                start = current;
                
                
            }   

            switch (*current){
                case '|':
                    start = current;
                    if(*(current + 1 ) == '|'){
                        val_len = 2;
                        array = AddToken(array, TOKEN_OR, start, val_len);
                        ++current;

                    }else if (*(current + 1 ) == '&'){
                        
                        val_len = 2;
                        array = AddToken(array, TOKEN_ERROR_PIPE, start, val_len);
                        ++current;
                    }
                     else {
                        val_len =1;
                        array = AddToken(array, TOKEN_PIPE, start, val_len);

                    }
    
                    break;
                case'&':
                start = current;
                 if(*(current + 1) == '&'){
                        val_len = 2;
                        array = AddToken(array, TOKEN_AND, start, val_len);
                        ++current;
                    } else {
                        val_len =1;

                        array = AddToken(array, TOKEN_BG, start, val_len);
                    }
                    break;
                    // не входят в базу: перенаправления с явным номером файлового дескриптора
                    // (`2> файл`, `2>&1`, `n>&-`),    
                case'<':
                    start = current;
                    val_len =1;
                    array = AddToken(array, TOKEN_REDIRECT_IN, start, val_len);     
                    break;
                case'>':
                    start = current;
                    if(*(current + 1) == '>'){
                        val_len = 2;
                        array = AddToken(array,TOKEN_REDIRECT_D_OUT , start, val_len);
                        ++current;
                    } else {
                        val_len =1;
                        array = AddToken(array, TOKEN_REDIRECT_OUT, start, val_len);
                    }
                    break;
                case';':
                        start = current;
                        val_len =1;
                        array = AddToken(array, TOKEN_SEQ, start, val_len);
                    break;
                case'(':
                    start = current;
                    val_len =1;
                    array = AddToken(array, TOKEN_OPEN_BRACKET, start, val_len);
                    break;
                case')':
                    start = current;
                    val_len =1;
                    array = AddToken(array, TOKEN_CLOSE_BRACKET, start, val_len);
                    break;
                case'\n':
                    start = current;
                    val_len =1;
                    array = AddToken(array, TOKEN_SEQ, start, val_len);
                    break;
                default:
                    break;
                }
            
                start = current ;
               
        }
       
    }
        
        ++current;
        is_skiped =0;
    }
    if(state != NORMAL){
        destroy_tokens(array);
        write(STDERR_FILENO,"2",1);
        return NULL;
        
    }
    return array;
}
void print_tokens(TokenArray *list){
    
   // printf("Start of printing tokns \n");
    if (list == NULL || list->head == NULL) {
        printf("NULL head \n");
        return;
    }
    for(int i = 0; i < list->cur_index; ++i){
        if(list->head[i].type == TOKEN_WORD || list->head[i].type == TOKEN_COMMENT ){
            printf("Token type: %s, value: %s\n", GetTokenType(list->head[i].type), list->head[i].value);
        }else{
            printf("Token type: %s, value: %s\n", GetTokenType(list->head[i].type), GetTokenValue(list->head[i].type));
        }
        
    }
}
TokenArray *CreateTokenArray(){
    TokenArray *arr =malloc(sizeof(TokenArray));
    arr->head = malloc(sizeof(Token) * ARRAY_INITIAL_SIZE); 
    arr->cur_index = 0;
    arr->cur_size = ARRAY_INITIAL_SIZE;
    return arr; 
}
TokenArray* AddToken(TokenArray *arr, TokenType type, const char *value, int value_length){
    if(arr == NULL){
        arr = CreateTokenArray();
    }
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
    if(arr->cur_index + 1  >= arr->cur_size){
        arr->cur_size *= 2;
        arr->head = Resize_Token_Array(arr->head, arr->cur_size);
    }
    arr->head[arr->cur_index++] = *new_token;
    return arr;
}
/*TokenArray* push_token_from_tail(TokenArray *list, TokenType type, const char *value, int value_length){
    if(list == NULL){
        list = CreateTokenList();
    }
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
*/
void destroy_tokens(TokenArray *list){ 
    if (list == NULL) {
        return;
    }
    for(int i = 0; i < list->cur_index; i++){
        free(list->head[i].value);
    }
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
const char *GetTokenValue(TokenType state) {
    switch (state) {
        case TOKEN_WORD:            return "WORD";
        case TOKEN_COMMENT:         return "COMMENT";
        case TOKEN_PIPE:            return "|";
        case TOKEN_ERROR_PIPE:      return "|&";
        case TOKEN_BG:              return "&";
        case TOKEN_SEQ:             return ";";
        case TOKEN_AND:             return "&&";
        case TOKEN_OR:              return "||";  
        case TOKEN_REDIRECT_IN:     return "<";
        case TOKEN_REDIRECT_D_OUT:  return ">>";
        case TOKEN_REDIRECT_OUT:    return ">";
        case TOKEN_OPEN_BRACKET:    return "(";
        case TOKEN_CLOSE_BRACKET:   return ")";
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
                default:
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
                default:
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
                default:
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

