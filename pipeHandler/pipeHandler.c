#include "pipeHandler.h"
/*
struct PipeSeparatedCommands {
    char command[MAX_COMMAND_LENGTH];
    char args[MAX_ARGS];
    int numArgs;
    int input_fd;
    int output_fd;
};
*/
int pipe_separated_commands_populator(char* token,struct PipeSeparatedCommands pipe_separated_commands[]){
    
    // tokenise wrt pipe '|'
    char* separated_command = strtok(token,"|");
    int i=0;
    while(separated_command!=NULL){
        strcpy(pipe_separated_commands[i].command,separated_command);
        separated_command = strtok(NULL,"|");
        i++;
    }
    // printf("%d\n",i);
    // print pipe separated commands for debugging
    // for(int j=0;j<i;j++){
    //     printf("%s",pipe_separated_commands[j].command);
    // }

    // args[arg_count] = strtok(temp, " \t\n");
    // while (args[arg_count] != NULL && arg_count < MAX_ARGS) {
    //     arg_count++;
    //     args[arg_count] = strtok(NULL, " \t\n");
    // }

    // tokenise wrt whitespace
    for(int j=0 ; j<i ; j++){
        int k=0;
        pipe_separated_commands[j].args[k] = strtok(pipe_separated_commands[j].command," \t");
        while( pipe_separated_commands[j].args[k]!=NULL && pipe_separated_commands[j].numArgs < MAX_ARGS ){
            k++;
            pipe_separated_commands[j].args[k] = strtok(NULL," \t");
        }
        pipe_separated_commands[j].numArgs = k;
        // set input and output file descriptors to STDIN
        pipe_separated_commands[j].pipe_fds[0] = STDIN_FILENO;
        pipe_separated_commands[j].pipe_fds[1] = STDOUT_FILENO;
    }

    // // print args for debugging
    // for(int j=0;j<i;j++){
    //     for(int k=0;k<pipe_separated_commands[j].numArgs;k++){
    //         printf("here: args %d -> %s ",k,pipe_separated_commands[j].args[k]);
    //     }
    //     printf("\n");
    // }

    // return number of pipe-separated commands
    return i;
}
