# include "activities.h"

/*
This specification requires you to print a list of all the processes currently running that were spawned by your shell in 
lexicographic order. This list should contain the following information about all processes :

    Command Name
    pid
    state : running or stopped
*/

void activities(struct BackgroundProcess background_processes[], int num_background_processes){

    printf("List of all background processes spawned by this shell:\n");
    printf("Command Name\t\tPID\t\tState\n");
    for(int i=0; i<num_background_processes; i++){
        printf("%s\t\t\t%d\t\t%s\n", background_processes[i].name, background_processes[i].pid, background_processes[i].status);
    }
    printf("\n");

}