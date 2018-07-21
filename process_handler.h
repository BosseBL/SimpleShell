pid_t new_process();
int init_process_handler();
int cleanup_process_handler();
void terminate_all();
int wait_for_process(pid_t, int);