#include "process_handler_private.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

pid_t new_process();
int init_process_handler();



typedef struct {
    pid_t pid;
    int status;
}process;

pid_t process_list[20];
int process_index;


void register_sighandler(int signal_code, void (*handler)(int sig) );
void interrupt_handler(int signal_code);
void child_handler(int signal_code);
int wait_for_process(pid_t, int);



int init_process_handler(int option) {
    
    if (option == 1) {
        register_sighandler(SIGCHLD, child_handler);
    }
    
    register_sighandler(SIGINT, interrupt_handler);

    return 0;
}


int new_process() {
    pid_t childpid = fork();
    
    if(childpid == 0) {
#ifdef SIGNALDETECTION
        register_sighandler(SIGCHLD, SIG_DFL);
#endif
        register_sighandler(SIGINT, SIG_DFL);
    }
    
    if(childpid == -1) return -1;
    
    process_list[process_index + 1] = childpid;
    process_index++;
    
    return childpid;
}




void register_sighandler(int signal_code, void (*handler)(int sig) ) {
    int return_value;							// för returvärden från systemanrop
    
    struct sigaction signal_parameters;			// datastruktur för parametrar till systemanropet sigaction
    
    /*
     * ange parametrar för anrop till sigaction
     * sa_handler 	= den rutin som ska köras om signal ges
     * sa_mask		= mängd av övriga signaler som ska spärras
     * 					medan handlern körs (här: inga alls
     * sa_flags		= extravillkor (här: inga alls)
     */
    
    signal_parameters.sa_handler = handler;
    sigemptyset( &signal_parameters.sa_mask );	// skapar tom mängd
    signal_parameters.sa_flags = 0;
    // begär hantering av signal_code med ovanstående parametrar
    return_value = sigaction( signal_code, &signal_parameters, (void *) 0);
    
    if(return_value == -1) {					// sigaction() misslyckades
        perror("sigaction() failed");
        exit(1);
    }
}


void terminate_all() {
    /* avbryter alla processe */
    int i;
    for (i = 0; i <= process_index; i++) {
        kill(process_list[i], SIGINT);
    }
    
    /* väntar på att de har avslutats */
    while (process_index >= 0) {
        wait_for_process(-1, 0);
    }
}




void interrupt_handler(int signal_code) {
        terminate_all();
}

void child_handler(int signal_code) {
    pid_t p = wait_for_process(-1, WNOHANG);
    while (p > 0) {
        p = wait_for_process(-1, WNOHANG);
    }
}



int wait_for_process(pid_t p, int option) {
    int return_pid;
    pid_t pid = p;
    int status;
    
    /* väntar på att process pid ska avsluta*/
    return_pid = waitpid( pid, &status, option );
    
    
    if( return_pid == -1 ) {
        fprintf( stderr, "in wait_for_process() while waiting for Child (pid %ld), errno: %d\n", (long int) pid, errno);
        return -1;
    }
    
    
    int i;
    for (i = 0 ; i <= process_index ; i++) {
        if(process_list[i] == return_pid) {
            while (i < process_index) {
                process_list[i] = process_list[i+1];
                i++;
            }
            process_index--;
            break;
        }
    }
    
    
    /* undersöker med vilken status processen pid avslutades med */
    if( WIFEXITED( status ) ) /* processen pid har kört klart */
    {
        int child_status = WEXITSTATUS( status );
        if( child_status != 0 ) /* processen pid fick problem */ {
            fprintf( stderr, "in wait_for_process(), Child (pid %ld) failed with exit code %d,\n",
                    (long int) return_pid, child_status ); return -1;
        }
        fprintf(stderr, "[%ld] terminated with exit code %d\n", (long int) pid, child_status);
    }
    else {
        if( WIFSIGNALED( status ) ) /* processen pid avbröts av signal */ {
            int child_signal = WTERMSIG( status );
            fprintf( stderr, "in wait_for_process(), child (pid %ld) was terminated by signal no. %d,\n",
                    (long int) return_pid, child_signal ); return -1;
        }
    }
    
    
    /* returnerar 0 om processen pid avslutades normalt */
    return 0;
}


