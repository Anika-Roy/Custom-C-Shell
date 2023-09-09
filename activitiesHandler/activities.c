# include "activities.h"

/*
This specification requires you to print a list of all the processes currently running that were spawned by your shell in 
lexicographic order. This list should contain the following information about all processes :

    Command Name
    pid
    state : running or stopped
*/

void activities(struct BackgroundProcess background_processes[], int num_background_processes){

    for(int i=0; i<num_background_processes; i++){
        printf("%s : %d - %s\n", background_processes[i].name, background_processes[i].pid, background_processes[i].status);
    }
    // printf("\n");
    return;
}