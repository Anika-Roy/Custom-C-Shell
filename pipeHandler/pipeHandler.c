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

    // check if the last token has only whitespaces
    // if yes, then decrement i
    int last_token_length = strlen(pipe_separated_commands[i - 1].command);
    int last_token_is_whitespace = 1;
    for (int j = 0; j < last_token_length; j++) {
        if (pipe_separated_commands[i - 1].command[j] != ' ' && pipe_separated_commands[i - 1].command[j] != '\t') {
            last_token_is_whitespace = 0;
            break;
        }
    }
    if (last_token_is_whitespace) {
        i--;
    }

    // printf("%d\n",i);
    // // print pipe separated commands for debugging
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

    // print args for debugging
    // for(int j=0;j<i;j++){
    //     for(int k=0;k<pipe_separated_commands[j].numArgs;k++){
    //         printf("here %d: args %d -> %s \n",j,k,pipe_separated_commands[j].args[k]);
    //     }
    //     printf("\n");
    // }
    // printf("%d\n",i);

    // return number of pipe-separated commands
    return i;
}

void get_input_output_fds(struct PipeSeparatedCommands pipe_separated_commands[],int* input_fd,int* output_fd,int num_pipes){
    
    // check for file redirection if it is the first pipe
    int k=0;

    char* input_file = NULL;
    // [TODO: Error handling]
    for (int i = 0; i < pipe_separated_commands[k].numArgs; i++) {
        if (strcmp(pipe_separated_commands[k].args[i], "<") == 0) {
            // Found input redirection symbol '<'
            if (i + 1 < pipe_separated_commands[k].numArgs) {
                input_file = pipe_separated_commands[k].args[i + 1];
                pipe_separated_commands[k].args[i] = NULL; // Null-terminate the command
                // break;
            } else {
                printf("Error: Missing input file after '<'\n");
                exit(EXIT_FAILURE);
            }
        } 
    }

    if (input_file) {
        // Open the input file and associate it with STDIN_FILENO
        *input_fd = open(input_file, O_RDONLY);
        if (*input_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        // dup2(input_fd, STDIN_FILENO);
        // close(input_fd);
    }

    k= num_pipes-1;
    // check for file redirection if it is the last pipe
    
    char* output_file = NULL;
    // [TODO: Error handling]
    for (int i = 0; i < pipe_separated_commands[k].numArgs; i++) {
        if (strcmp(pipe_separated_commands[k].args[i], ">") == 0) {
            // Found output redirection symbol '>'
            if (i + 1 < pipe_separated_commands[k].numArgs) {
                output_file = pipe_separated_commands[k].args[i + 1];
                pipe_separated_commands[k].args[i] = NULL; // Null-terminate the command
                break;
            } else {
                printf("Error: Missing output file after '>'\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (output_file) {
        // printf("output file: %s\n",output_file);
        // Open the output file and associate it with STDOUT_FILENO
        *output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (*output_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        // dup2(output_fd, STDOUT_FILENO);
        // close(output_fd);
    }
    return;
    
    
}