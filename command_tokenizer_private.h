
#define TRUE ( 1 )
#define FALSE ( 0 )


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char** token_list;


char** tokenize(char*);
char* next_token();
void cleanup_tokenizer();

char* eat_until(char*, char);
char* eat_space(char*);
char* eat_word(char*);
char* eat_special(char*);

int count_tokens(char*);
char* get_span(char* from, char* too);