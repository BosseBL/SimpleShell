
#include "command_exec_private.h"
#include "command_parser.h"
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#define PIPE_READ_SIDE ( 0 )
#define PIPE_WRITE_SIDE ( 1 )
#define FALSE ( 0 )

int exec_command_rec(int*);



int exec_command(int argc, char** command) {
    
    init_parser(argc, command);
    
    int dummy_pipe_filedesc[2];
    int return_value;
    
    return_value = pipe(dummy_pipe_filedesc);
    if (return_value == -1) return -1;
    
    return_value = exec_command_rec(dummy_pipe_filedesc);
    if (return_value == -1) return -1;
    
    cleanup_parser();
    
    return 0;
}


int exec_command_rec(int last_pipe_filedesc[]) {
    int next_pipe_filedesc[2];
    int return_value;
    pid_t childpid;
    char** exec;
    
    get_next_param(&exec);
    
    if (exec[0] == NULL) {
        return 0;
    }
    
    return_value = pipe(next_pipe_filedesc);
    
    childpid = fork();
    
    if(childpid == 0) {
        if (has_first_param() == FALSE) {
            fprintf(stderr, "first");
            return_value = dup2(last_pipe_filedesc[PIPE_READ_SIDE], fileno(stdin));
            if(return_value == -1) exit(1);
        }
        if (has_last_param() == FALSE) {
            return_value = dup2(next_pipe_filedesc[PIPE_WRITE_SIDE], fileno(stdout));
            if(return_value == -1) exit(1);
        }
    
        return_value = close_pipes(2, last_pipe_filedesc, next_pipe_filedesc);
        if(return_value == -1) exit(1);
        
        (void) execvp(exec[0], exec);
        
        exit(1);
    }
    else {
        if(childpid == -1) return -1;
        
        return_value = close_pipes(1, last_pipe_filedesc);
        if(return_value == -1) return -1;
        
        if(has_last_param() == FALSE) {
            return_value = exec_command_rec(next_pipe_filedesc);
            if(return_value == -1) return -1;
        }
        
        return_value = wait_for_child(childpid);
        if(return_value == -1) return -1;
        
        
        return 0;
    
    }
    
}





int close_pipes(int num, ...) {
    /* deklarerar en variable argument list */
    va_list pipe_list;
    
    /* deklarerar en h�llare f�r addressen till n�sta pipe fildeskriptor */
    int *pipe_filedesc;
    
    int return_value;
    
    /* initierar variable argument list */
    va_start(pipe_list, num);
    
    /* ittererar genom parametrarna */
    int i;
    for (i = 0; i < num; i++) {
        
        /* h�mtar n�sta pipe fildeskriptor fr�n parameter listan */
        pipe_filedesc = va_arg(pipe_list, int*);
        
        /* st�nger b�da sidorna av pipen */
        
        return_value = close( pipe_filedesc[PIPE_READ_SIDE] );
        if( return_value == -1 ) {
            fprintf(stderr, "in close_pipes(), while closing pipe read side (filedesc %ld),\n", (long int) pipe_filedesc[PIPE_READ_SIDE]);
            return -1;
        }
        return_value = close( pipe_filedesc[PIPE_WRITE_SIDE] );
        if( return_value == -1 ) {
            fprintf(stderr, "in close_pipes(), while closing pipe write side (filedesc %ld),\n", (long int) pipe_filedesc[PIPE_WRITE_SIDE]);
            return -1;
        }
    }
    /* st�dar upp variable argument list och returnerar*/
    va_end(pipe_list);
    return 0;

}


int wait_for_child(pid_t pid) {
    int status;
    int return_value;
    
    /* v�ntar p� att process pid ska avsluta*/
    return_value = waitpid( pid, &status, 0 );
    if( return_value == -1 ) {
        fprintf( stderr, "in wait_for_child() while waiting for Child (pid %ld),\n", (long int) pid);
        return -1;
    }
    
    /* unders�ker med vilken status processen pid avslutades med */
    if( WIFEXITED( status ) ) /* processen pid har k�rt klart */
    {
        int child_status = WEXITSTATUS( status );
        if( child_status != 0 ) /* processen pid fick problem */ {
            fprintf( stderr, "in wait_for_child(), Child (pid %ld) failed with exit code %d,\n",
                    (long int) pid, child_status ); return -1;
        }
    }
    else {
        if( WIFSIGNALED( status ) ) /* processen pid avbr�ts av signal */ {
            int child_signal = WTERMSIG( status );
            fprintf( stderr, "in wait_for_child(), child (pid %ld) was terminated by signal no. %d,\n",
                    (long int) pid, child_signal ); return -1;
        }
    }
    /* returnerar 0 om processen pid avslutades normalt */
    return 0;
}

