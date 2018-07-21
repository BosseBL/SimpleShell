
#include "command_tokenizer_private.h"


char** tokenize(char* str) {
    if(token_list != NULL) cleanup_tokenizer();
    
    int n_tok = count_tokens(str);
    token_list = (char**)malloc( (n_tok + 1) * sizeof(char*) );
    if(token_list == NULL) return NULL;
    
    int i;
    for(i = 0 ; i < n_tok ; i++) {
        token_list[i] = next_token(&str);
    }
    token_list[i] = NULL;
    
    return token_list;
}


int count_tokens(char str[]) {
    int count = 0;
    
    while (str[0] != '\0') {
        switch (str[0]) {
            case '"': case '\'':
                str = eat_until((char*)(str+1) , str[0]);
                count++;
                break;
            case ' ': case '\n':
                str = eat_space(str);
                break;
            case '>': case '<': case '|': case '&':
                str = eat_special(str);
                count++;
                break;
            default:
                str = eat_word(str);
                count++;
                break;
        }
    }
    return count;
}


char* eat_until(char str[], char c) {
        int i = 0;
        while (str[i] != '\0' && str[i] != c) {
            if(str[i] == '\0') return NULL;
            i++;
        }
        return (char*)(str + i + 1);
}

char* eat_word(char str[]) {
    int i = 0;
    while (str[i] != ' ' && str[i] != '\n' && str[i] != '\0' && str[i] != '|' && str[i] != '<' && str[i] != '>' && str[i] != '&') {
        i++;
    }
    return (char*)(str + i);
}

char* eat_space(char str[]) {
    int i = 0;
    while (str[i] == ' ' || str[i] == '\n') {
        i++;
    }
    return (char*)(str + i);
}

char* eat_special(char str[]) {
    if (str[0] == '|' || str[0] == '<' || str[0] == '>' || str[0] == '&') {
        return (char*)(str + 1);
    }
    else return NULL;
}


/* ger strängen från och med pekare "from" fram till pekare "too" */
char* get_span(char* from, char* too) {
    int length = (int)(too - from);
    char* return_str = (char*)malloc( (int)(length + 1) * sizeof(char) );
    if (return_str == NULL) return NULL;
    int i;
    for (i = 0; i < length; i++) {
        return_str[i] = from[i];
    }
    return_str[i] = '\0';
    return return_str;
}


char* next_token(char* *str) {
    char* next_str;
    char* token;
    
    *str = eat_space(*str);
    
	if(str[0] != '\0') {
        switch (*str[0]) {
            case '"': case '\'':
                next_str = eat_until((char*)(*str+1), *str[0]);
                break;
            case '>': case '<': case '|': case '&':
                next_str = eat_special(*str);
                break;
            default:
                next_str = eat_word(*str);
                break;
        }
        token = get_span(*str, next_str);
        *str = next_str;
        return token;
	}
	return NULL;
}

void cleanup_tokenizer() {
    int i = 0;
    while (token_list[i] != NULL) {
        free(token_list[i]);
        i++;
    }
    free(token_list);
	token_list = NULL;
}







