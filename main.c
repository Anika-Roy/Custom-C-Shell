#include "headers.h"
#include "promptHandler/prompt.h"
#include "warpHandler/warp.h"
#include "peekHandler/peek.h"
#include "pasteventsHandler/pastevents.h"

// Function to execute background commands -->   ChatGPT referred
void execute_background_commands(char* tokens[], int i){
    // // Create a new process using fork
    // pid_t pid = fork();

    // if (pid == -1)
    // {
    //     perror("fork");
    //     exit(EXIT_FAILURE);
    // }
    // else if (pid == 0) // Child process
    // {
    //     // Tokenize the command for exec
    //     char *command = tokens[0];
    //     char *args[64]; // Adjust the size if needed

    //     char *token = strtok(command, " ");
    //     int arg_index = 0;
    //     while (token != NULL)
    //     {
    //         args[arg_index++] = token;
    //         token = strtok(NULL, " ");
    //     }
    //     args[arg_index] = NULL;

    //     // Execute the command using exec
    //     execvp(args[0], args);
        
    //     // If execvp returns, an error occurred
    //     perror("execvp");
    //     exit(EXIT_FAILURE);
    // }
    // else // Parent process
    // {
    //     // Print the process id of the child process
    //     printf("Process id of the child process: %d\n", pid);
    // }
}

// Function to execute sequential commands -->   ChatGPT referred
void execute_sequential_commands(char* tokens[], int count) {
    // for (int i = 0; i < count; i++) {
    //     // Fork a new process
    //     pid_t pid = fork();
    //     if (pid == -1) {
    //         perror("fork");
    //         exit(EXIT_FAILURE);
    //     } else if (pid == 0) { // Child process
    //         // Tokenize the command
    //         char *args[64]; // Adjust size if needed
    //         char *token = strtok(tokens[i], " ");
    //         int arg_index = 0;
    //         while (token != NULL) {
    //             args[arg_index++] = token;
    //             token = strtok(NULL, " ");
    //         }
    //         args[arg_index] = NULL;

    //         // Execute the command using execvp
    //         execvp(args[0], args);
            
    //         // If execvp returns, an error occurred
    //         perror("execvp");
    //         exit(EXIT_FAILURE);
    //     } else { // Parent process
    //         // Wait for the child process to finish
    //         wait(NULL);
    //     }
    // }
}

int main()
{
    // Keep accepting commands
    char store_calling_directory[1024];
    getcwd(store_calling_directory, sizeof(store_calling_directory));

    // store the previous directory for warp
    char store_previous_directory[1024];
    getcwd(store_previous_directory, sizeof(store_previous_directory));

    // get the absolute path for the history file <FILENAME> and store it
    char history_file_path[1024];
    strcpy(history_file_path, store_calling_directory);
    strcat(history_file_path, "/");
    strcat(history_file_path, FILENAME);

    // Read past events from history_file_path
    char events[MAX_EVENTS][MAX_EVENT_LENGTH];
    int event_count=0;
    read_past_events(events, &event_count, history_file_path);   

    // print events for debugging
    // for(int i=0;i<event_count;i++){
    //     printf("%d->%s\n",i,events[i]);
    // }

    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(store_calling_directory);
        char input[4096];
        fgets(input, 4096, stdin);

        // Dclare a variable to store the original command, which we'll get by concatenating all tokens
        char original_command[MAX_EVENT_LENGTH];
        original_command[0]='\0';

       // declare array of structs to store tokens and their delimiters
        struct TokenWithDelimiter tokens[MAX_TOKENS];
        
        int i=tokeniser(tokens,input);

        int flag=1;
        int execute=0;
        int token_count=0;

        char delimiter;
        char *args[MAX_ARGS];
        int arg_count = 0;
        // execute all tokens in a loop (autocompleted by Copilot)
        for (int j = 0; j < i; j++) {   
            // get delimiter
            delimiter=tokens[j].delimiter;

            char command[MAX_COMMAND_LENGTH];

            // Tokenize with all whitespaces (space and tab) to get command and arguments
            if(execute==0){
                arg_count = 0;

                args[arg_count] = strtok(tokens[j].token, " \t\n");
                while (args[arg_count] != NULL && arg_count < MAX_ARGS) {
                    arg_count++;
                    args[arg_count] = strtok(NULL, " \t\n");
                }   
            }
            execute=0;

            // Concatenate all tokens(and their arguments) to get the original command(along with delimiter)
            if(strcmp(args[0],"pastevents")!=0){
                strcat(original_command, args[0]);
                for (int k = 1; k < arg_count; k++) {
                    strcat(original_command, " ");
                    strcat(original_command, args[k]);
                }
                if(token_count<i-1)
                    strcat(original_command, &delimiter);
                token_count++;
            }

            // If pastevents was present, check if second arg was execute, only then save it
            if((strcmp(args[0],"pastevents")==0 && arg_count>1 && strcmp(args[1],"execute")!=0) || (strcmp(args[0],"pastevents")==0 && arg_count==1) ){
                flag=0;
            }
            
            // Now you have the command and its arguments in the args array
            if(strcmp(args[0],"exit")==0){
                exit(0);
            }

            // If the command is warp, call the warp function
            else if (strcmp(args[0], "warp") == 0) {
                //check for delimiter[TODO]
                warp(args, arg_count, store_calling_directory, store_previous_directory);
            }

            // If the command is peek, call the peek function
            else if (strcmp(args[0], "peek") == 0) {
                //check for delimiter[TODO]
                peek(args, arg_count, store_previous_directory,store_calling_directory);
            }

            else if(strcmp(args[0],"pastevents")==0){
                /*
                if its pastevents execute <index>, 
                replace the command in the args array with the command at index <index>, 
                decrement j and run the loop again
                This way, we don't store it in the history and we can execute it
                */ 
                if(arg_count>1 && strcmp(args[1],"execute")==0){
                    // args[2]="<index>"
                    int index=atoi(args[2]);
                    if(index>event_count){
                        printf("Index out of bounds\n");
                        continue;
                    }
                    strcpy(command,events[index-1]);
                    // printf("%s\n",command);
                    char* command_args[100];
                    int command_arg_count=0;
                    char* token=strtok(command," ");
                    while(token!=NULL){
                        command_args[command_arg_count]=token;
                        command_arg_count++;
                        token=strtok(NULL," ");
                    }
                    // printf("%s\n",command_args[0]);
                    // replace 
                    strcpy(args[0],command_args[0]);
                    for(int k=1;k<command_arg_count;k++){
                        strcpy(args[k],command_args[k]);
                    }
                    arg_count=command_arg_count;
                    j--;
                    // printf("args0:%s\n",args[0]);
                    execute=1;
                    continue;
                }
                else if(arg_count>1 && strcmp(args[1],"purge")==0){
                    event_count=0;
                    write_past_events(events,event_count,history_file_path);
                    continue;
                }
                else {
                    if(arg_count==1){
                        for(int i=0;i<event_count;i++){
                            printf("%s\n",events[i]);   
                        }
                    }
                }
            }

            else{
                // execute using execvp
                // execvp(args[0], args);
            }
        }   
        // if not, add the input to the list of past events
        // also, if the original command contains the work 'pastevents'(apart from ), don't add it to the list
        if (flag) {
            // printf("reached here");
            if (event_count == 0 || strcmp(events[event_count - 1], original_command) != 0) {
                // printf("%s\n", original_command);
                // strcat(original_command, "\n");
                add_event(original_command, events, &event_count);
                // for(int i=0;i<event_count;i++){
                //     printf("%d->%s\n",i,events[i]);
                // }
                write_past_events(events, event_count, history_file_path);
            }
        }
        // print events array
        // for(int i=0;i<event_count;i++){
        //     printf("%d->%s\n",i,events[i]);
        // }

    }

    return 0;
}
