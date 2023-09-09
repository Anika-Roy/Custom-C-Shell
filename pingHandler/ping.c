#include "ping.h"

void ping(pid_t pid, int signal_number){

    // Check if the process with the specified PID exists
    if (kill(pid, 0) == -1) {
        printf("No such process found\n");
        // exit(EXIT_FAILURE);
        return;
    }

    signal_number = signal_number % 32;

    int kill_status = kill(pid, signal_number);
    if(kill_status == -1){
        printf("Signal couldn't be sent\n");
    }
    else{
        printf("Sent signal %d to process with pid %d\n", signal_number, pid);
    }
    // exit(EXIT_SUCCESS);
    return;
}