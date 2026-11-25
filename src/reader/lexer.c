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
   
};

bool check_meta(unsigned char c) {
    return is_meta[c]; 
}

TokenArray* tokenize(char **text_of_input_ptr){
    start_of_tokenize:
    if(text_of_input_ptr == NULL){
        return NULL;
    }
    char *tmp_text;
    char *text_of_input = *text_of_input_ptr;
    //printf("Tokenize start\n");
    char *current = text_of_input, *start = text_of_input;
    TokenArray * array = CreateTokenArray(); 
    StateType state = NORMAL;
    
    int val_len = 0;
    bool is_skiped = false;
    int result;
    while(*current != '\0'){
        if((check_meta(*start)) && start < current ){ /// Ставаит start на начало след слова или спец символа,
            start = current;                            // если start был спецсивмолом, а current уже не спецсимвол
        }
        if(state == IN_BACKSLASH){
            state = NORMAL;
            //значит ввели \\n и нужно дочитать ввод
            if(*current == '\n'){ 
                free_tokens(array); // Будем токенезировать заново в любом случае
                if(config.interactive)
                    write(STDOUT_FILENO,"> ", 2);
                if(*(current + 1) == '\0'){ //строка получена через read_input 
                    // нужно сразу убрать \\n чтобы потом они не вызывали проблем 
                    *(current - 1) = '\0'; // отступаем на 1 символ \ \n   
                    tmp_text = read_input(0);
                    if(!tmp_text){ //EOF
                        errno = 2;
                        config.shell_status = 2;
                        if(config.interactive){
                            write(STDERR_FILENO,"mysh: lexer: unexpected EOF\n",strlen("mysh: lexer: unexpected EOF\n"));
                            return NULL;
                        }
                        exit(2);  
                    }
                    
                    *text_of_input_ptr = merge_input_strings(*text_of_input_ptr , tmp_text);
                    if (*text_of_input_ptr == NULL) {
                        errno = 2;
                        config.shell_status = 2;
                        if(config.interactive){
                            write(STDERR_FILENO,"mysh: lexer: allocation error\n",strlen("mysh: lexer: allocation error\n"));
                            return NULL;
                        }
                        exit(2);                    // EOF \Ctrl-D 
                    }
                    goto start_of_tokenize; // запускаем все заново

                }else{ //возможно только с флагом -c
                    //Сдвигаем всю строку после \\n на 2 назад
                    memmove(current - 1, current + 1, strlen(current + 1) + 1); //+1 в конце для \0
                    goto start_of_tokenize; // запускаем все заново
                }
                
            }
            if(*(current + 1) == '\0'){
                
                array = AddToken(array, TOKEN_WORD, start, current - start +1);
                is_skiped = true;
                ++current;
                continue;
            }else{
                is_skiped = true;
            }
        }else if (*current == '#' && ( current == start || check_meta(*(current - 1)))){ // проверка что в начале слова
            tokenise_commment(&current, &start); 
            if(*(current + 1 ) == '\0' ){
                ++current;
                continue;
            }
        
        }else if(*current == '\''){
            result = tokenise_single_quote(&current);
            if(result < 0){  
                goto quote_error;   
            }
            
        }else if(*current == '"'){
           result = tokenise_double_quote(&current);
            if(result < 0){
                if(result == -2)
                    goto quote_continue;   
                goto quote_error;   
            }

        }else if(*current == '\\'){
            state = IN_BACKSLASH;
        }    
    if(!is_skiped)
    {    
        if(check_meta(*current) || *(current + 1 ) =='\0' ) // Должны запушить что нибудь               //*current ==' ' || *current == '\t' || *current == '\n' 
            {  
            /// WORD
            if(!check_meta(*start)){ //Пушим слово
                val_len = current - start;
                if((*(current + 1 ) =='\0' )&& !check_meta(*current)){ // проверка того что слово именно в конце строки
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
                    array = AddToken(array, TOKEN_NEWLINE, start, val_len);
                    break;
                default:
                    break;
                }
               
                start = current ;
               
        }
       
    }
        
        ++current;
        is_skiped = false;
    }
    
    return array;
    quote_continue:
        // с -с если кавычки закончились на \n то это ошибка
        if(config.command_str != NULL){
            goto quote_error; 
        }
        if(config.interactive){
            write(STDOUT_FILENO,"> ", 2);
        }
        tmp_text = read_input(0);
        free_tokens(array); 
        if(!tmp_text){ //EOF
            errno = 2;
            config.shell_status = 2;
            if(config.interactive){
                write(STDERR_FILENO,"mysh: lexer: unexpected EOF\n",strlen("mysh: lexer: unexpected EOF\n"));
                return NULL;
            }
            exit(2);  
        }
        *text_of_input_ptr = merge_input_strings(*text_of_input_ptr , tmp_text);
        if (*text_of_input_ptr == NULL) {
            config.shell_status = 2;
            errno = 2;
            return NULL;                    // EOF \Ctrl-D 
        }
        goto start_of_tokenize;
    quote_error:
        //если в интерактивном режиме то не падаем
        if(config.command_str == NULL){
            if(config.interactive)
                write(STDOUT_FILENO,"> ", 2);
            
            free_tokens(array);
            tmp_text = read_input(0);
            if(!tmp_text){ //EOF
                errno = 2;
                config.shell_status = 2;
                write(STDERR_FILENO,"mysh: lexer: unexpected EOF\n",strlen("mysh: lexer: unexpected EOF\n"));
                return NULL;
            }
            *text_of_input_ptr = merge_input_strings(*text_of_input_ptr , tmp_text);

            if (*text_of_input_ptr == NULL) {
                config.shell_status = 2;
                errno = 2;
                //если читаем с файла то сразу ошибка
                write(STDERR_FILENO,"mysh: lexer: quote_error\n",strlen("mysh: lexer: quote_error\n"));
                return NULL;                    // EOF \Ctrl-D 
            }
            goto start_of_tokenize;
        }else{
            config.shell_status = 2;
            free_tokens(array);
            write(STDERR_FILENO,"mysh: lexer: quote_error\n",strlen("mysh: lexer: quote_error\n"));
            return NULL;
        }
}
char *tokenise_commment( char **current, char **start){ //TokenArray *array,int val_len,
    *start = *current;
    while(*(*current + 1) != '\0' && **current != '\n'){
        ++*current;
    }
    if(*(*current + 1) == '\0' && **current != '\n'){
        return NULL;
    }
    *start = *current;
    return NULL;
    }
int tokenise_single_quote(char **current){
    if(*(*current + 1) != '\0'){
        ++*current;
    }else{
        return -1;
    }
    while(*(*current + 1) != '\0' && **current != '\''){    // Поскольку \ не имеет силы в одинрных кавычках, 
        ++*current;                                         // просто сдвигаем каретку до следующего появления одинарной кавычки или конца строки    
    }
    if(**current != '\''){
        return -1;
    }

    return 0;
}
int tokenise_double_quote(char **current){
    if(*(*current + 1) != '\0'){
        ++*current;
    }else{
        return -1;
    }
    while(*(*current + 1) != '\0' && **current != '"'){    
        if(**current == '\\' && (*(*current + 1) == '"' ||*(*current + 1) == '\\')){ // если встретили \", то пропускаем \ и продолжаем т.е пропуск "
            ++*current;
        } 
        ++*current;
    }
    // отслеживание именно \\n
    if(**current == '\n' && *(*current - 1) == '\\'){
        return -2;
    }
    if(**current != '"'){
        return -1;
    }
    return 0;
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
    printf("Capacity: %d Position: %d\n", list->cur_size, list->cur_index);
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
    Token *new_token = &(arr->head[arr->cur_index++]) ; //Изменяем сущ токен из массива
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
    if(arr->cur_index >= arr->cur_size){
        arr->cur_size *= 2;
        arr->head = Resize_Token_Array(arr->head, arr->cur_size);
    }
    
    return arr;
}
void free_tokens(TokenArray *list){ 
    if (list == NULL) {
        return;
    }
    for(int i = 0; i < list->cur_index; i++){
        free(list->head[i].value);
    }
    free(list->head);
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
        case TOKEN_NEWLINE:         return "NEWLINE";
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
        case TOKEN_NEWLINE:         return "\\n";
        default:                    return "UNKNOWN";
    }
}
// Копирование без кавычек и слэшей
char *strncpy_no_quotes( const char *old_value,  int old_value_length) {
    //printf("%d",old_value[0]);
    char *new_value;
    bool is_backslash = false;
    StateType state = NORMAL;
    new_value = malloc(sizeof(char)* old_value_length +1 );  //для \0
    
    char *new_ptr = new_value;
    const char *old_ptr = old_value;
    int written = 0;
    for (int i = 0; i < old_value_length; ++i, ++old_ptr) {
        if(!is_backslash){
            if(*old_ptr == '\\'){
                switch(state){
                    case NORMAL:
                        is_backslash = true ;
                        continue;
                    case IN_DOUBLE_QUOTE:
                        if( i + 1 < old_value_length &&(*(old_ptr + 1) == '"'|| *(old_ptr + 1) == '\\' || *(old_ptr + 1) == '\n')){
                            is_backslash = true;
                            if(*(old_ptr + 1) == '\n'){
                                ++i; 
                                ++old_ptr; // если в "" встречается \\n то их не пишем  
                                is_backslash = false;
                            }
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
        is_backslash = false;
    }
    new_value[written] = '\0';  //оставляем небольшой хвост который в любом случае уберется при помощи free 
    return new_value;
}

Token* Resize_Token_Array(Token* token, int size){
    Token* new_arr = (Token*)realloc(token, size * sizeof(Token));
    if (new_arr == NULL) {    
        free(token);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return new_arr;
}
TokenArray* merge_token_arrs(TokenArray *arr1, TokenArray *arr2){
    if(arr1 == NULL || arr2 == NULL ){
        return NULL ;
    }
    // расширяем массив если нужно
    while(arr1->cur_size < arr1->cur_index + arr2->cur_index){
        arr1->cur_size *= 2;
        arr1->head = Resize_Token_Array(arr1->head, arr1->cur_size);
    }
    // добавляем в первый начиная с последнего 
    for(int i = arr1->cur_index , j = 0; j < arr2->cur_index; ++i ,++j){
        arr1->head[i] = arr2->head[j];
        arr2->head[j].value = NULL;  // чтобы при free не удалились 
    }
    arr1->cur_index += arr2->cur_index; 
    free_tokens(arr2);
    return arr1 ;
}

TokenArray* lexer_restart(TokenArray* old_head){
        char *text_of_input = NULL;
        TokenArray* head;
        if(config.interactive)
            write(STDOUT_FILENO,"> ", 2);
        if(!config.command_str)
            text_of_input = read_input(0);
        
        if (text_of_input == NULL) {
            errno = 2;
            config.shell_status = 2;
            //free_tokens(old_head);
            write(STDERR_FILENO,"mysh: lexer: unexpected EOF\n",strlen("mysh: lexer: unexpected EOF\n"));
            return NULL;            
            
        }
        head = tokenize(&text_of_input);
        if(!head)
            return NULL;
        free(text_of_input);
        // Убираем /n в конце предыдущего массива если функция только для операций требующих 2 опреанда
        if(old_head->cur_index >= 2 &&(
           old_head->head[old_head->cur_index - 2].type == TOKEN_AND ||
           old_head->head[old_head->cur_index - 2].type == TOKEN_OR  ||
           old_head->head[old_head->cur_index - 2].type == TOKEN_PIPE||
           old_head->head[old_head->cur_index - 2].type == TOKEN_BG  ||   
           old_head->head[old_head->cur_index - 2].type == TOKEN_SEQ ||
           old_head->head[old_head->cur_index - 2].type == TOKEN_OPEN_BRACKET
        )){
            --old_head->cur_index; //затрем следующим вводом 
        }else{ //если дописываем () воспринимаем \n как ;
           old_head->head[old_head->cur_index - 1].type = TOKEN_SEQ; 
        }
        old_head = merge_token_arrs(old_head, head);
        return old_head;
}
TokenArray* lexer_test_mode(){
        char *text_of_input;
        TokenArray* head;
        printf("START_OF_PROGRAMM\n");
        printf("Start of reading\n");
        text_of_input = read_input(0);
        if (text_of_input == NULL) {
            exit(2);                    // EOF \Ctrl-D 
        }
        head = tokenize(&text_of_input);
        free(text_of_input);
        print_tokens(head);
        return head;
        // destroy_tokens(head);
}
