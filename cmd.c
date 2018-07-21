
/*echo, tee, cd, exit*/

#include <string.h>
#include <sys/types.h>	// definierar bl.a. typen pid_t
#include <errno.h>		// definierar felkontrollvariabeln errno
#include <signal.h>		// definierar signalnamn m.m.
#include <stdio.h>		// definierar stderr, dit felmeddelanden skrivs
#include <stdlib.h>		// definierar bl.a. exit()
#include <unistd.h>		// definierar bl.a. fork()
#include "command_exec.h"
#include "command_tokenizer.h"
#include "command_parser.h"
#include "process_handler.h"


#define TRUE ( 1 )
#define OVERWRITE ( 1 )
#define IF_N_DEF ( 0 )
#define SIGNALDETECTION

#define MAX_IN 70


int change_directory(char*);
void clean_n_exit();
void printall(char**);


int main(int argc, char** argv, char** envp) {
    char str[MAX_IN];               /* Input sparas i denna array */
    int n_of_tokens;                /* antalet tokens i input */
    char** tokens;                  /* token array */
    
    int return_value;
    pid_t childpid;
    

    
#ifdef SIGNALDETECTION
    return_value = init_process_handler(1);
#else
    return_value = init_process_handler(0);
#endif

    
    while (TRUE) {
        
        /* väntar oblockerande på eventuella bakgrundsprocesser */
        while (wait_for_process(-1, WNOHANG) > 0);
        
        
        /* Skriver input prompt (/current directory >>)*/
        printf("%s >>", getenv("PWD"));
        
        
        /* Hämtar input */
        fgets(str, MAX_IN, stdin);
        
        /* Gör tokens av input */
        tokens = tokenize(str);
        
        /* räknar antalet tokens */
        n_of_tokens = 0;
        while (tokens[n_of_tokens] != NULL) n_of_tokens++;
        
        printall(tokens);
        
        /* Fanns inga tokens i input loopar vi och ger en ny prompt */
        if (tokens[0] == NULL) {
            continue;
        }
        
        /* Kollar om input är ett inbyggt kommando och exikverar kommandot */
        
        if(strcmp(tokens[0], "cd") == 0 ) {
            return_value = change_directory(tokens[1]);
            if(return_value == -1) {
                printf("failed to chdir: %u\n", errno);
            }
            continue;
        }
        
        if( strcmp(tokens[0], "exit") == 0 ) break;
        

        
        /* Skapar ny process med process_handler */
        childpid = new_process();
        
        /* ------------------- Child process --------------------*/
        
        if( childpid == 0 ) {
            return_value = exec_command(n_of_tokens, tokens);
            if(return_value == -1) {
                perror("exec_command() in main");
                exit(1);
            }
            exit(0);
        }
        
        
        /* -------------------- Parent process ------------------*/
        
        /* Om processen inte gick att skapa */
        if(childpid == -1) {
            perror("new_process in main");
            exit(1);
        }
      

        
        
        /* Om sista strängen i kommandot inte är "&" väntar vi på att processen avslutas */
        if (strcmp(tokens[n_of_tokens - 1], "&") ) {
            printf("& fount\n");
            wait_for_process(childpid, 0);
        }
        
        
    }
    
    
    
    
    cleanup_tokenizer();
    // cleanup_process_handler();
    exit(0);
}


int change_directory(char* str) {
    int r_value;
    char* dir;
    if (str == NULL) dir = getenv("HOME");
    else if (str[0] == '/') dir = str;
    else {
        dir = getenv("PWD");
        if(strcmp(dir, "/")) dir = strcat(dir, "/");
        dir = strcat(dir, str);
    }
    r_value = chdir(dir);
    if (r_value == 0) {
        r_value = setenv("PWD", dir, 1);
    }
    return r_value;
}



void clean_n_exit() {
    cleanup_tokenizer();
    exit(0);
}



void printall(char** p) {
    int i = 0;
    printf("\n{");
    while (p[i] != NULL) {
        printf("[%s], ", p[i]);
        i++;
    }
    printf("[%s] }\n", p[i]);
}



