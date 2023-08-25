#include "headers.h"
#include "promptHandler/prompt.h"
#include "warpHandler/warp.h"
#include "peekHandler/peek.h"
#include "pasteventsHandler/pastevents.h"

#define MAX_TOKENS 1024
#define MAX_ARGS 64

#define MAX_EVENTS 15
#define FILENAME "pastevents.txt"

#define MAX_EVENT_LENGTH 1000

void add_event(const char *event, char events[][MAX_EVENT_LENGTH], int *count) {
    if (*count >= MAX_EVENTS) {
        // Remove the oldest event
        for (int i = 1; i < MAX_EVENTS; i++) {
            strcpy(events[i - 1], events[i]);
        }
        (*count)--;
    }
    strcpy(events[*count], event);
    (*count)++;
}

void read_past_events(char events[][MAX_EVENT_LENGTH], int *count, const char *history_file_path) {
    FILE *file = fopen(history_file_path, "r");
    if (file != NULL) {
        *count = 0;
        while (*count < MAX_EVENTS && fgets(events[*count], sizeof(events[*count]), file)) {
            (*count)++;
        }
        fclose(file);
    }
}

void write_past_events(const char events[][MAX_EVENT_LENGTH], int count, const char *history_file_path) {
    FILE *file = fopen(history_file_path, "w");
    if (file != NULL) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s\n", events[i]);
        }
        fclose(file);
    }
}

struct TokenWithDelimiter {
    char *token;
    char delimiter; // Stores either ';' or '&'
};

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

        // tokenise the input --> store in tokens as structs (autocompleted by Copilot)
        // remove empty tokens obtained at the end
        char *token = strtok(input, ";&\n");
        struct TokenWithDelimiter tokens[MAX_TOKENS];
        int i = 0;
        while (token != NULL)
        {
            tokens[i].token = token;
            tokens[i].delimiter = input[strlen(token)];
            token = strtok(NULL, ";&\n");
            i++;
        }

        // print the tokens for debugging
        for (int j = 0; j < i; j++) {
            printf("%d->%s%c\n",j, tokens[j].token, tokens[j].delimiter);
        }

        int flag=1;
        // execute all tokens in a loop (autocompleted by Copilot)
        for (int j = 0; j < i; j++) {   

            // Tokenize with all whitespaces (space and tab) to get command and arguments
            char *command = tokens[j].token;
            char *args[MAX_ARGS];
            int arg_count = 0;

            args[arg_count] = strtok(command, " \t\n");
            while (args[arg_count] != NULL && arg_count < MAX_ARGS) {
                arg_count++;
                args[arg_count] = strtok(NULL, " \t\n");
            }

            // print the command and arguments for debugging
            // printf("Command: %s\n",100 args[0]);
            // printf("Arguments: \n");
            // for (int k = 1; k < arg_count; k++) {
            //     printf("%s ", args[k]);
            // }
            // printf("\n");

            // Concatenate all tokens(and their arguments) to get the original command(along with delimiter)
            if(strcmp(args[0],"pastevents")!=0){
                strcat(original_command, args[0]);
                for (int k = 1; k < arg_count; k++) {
                    strcat(original_command, " ");
                    strcat(original_command, args[k]);
                }
                strcat(original_command, &tokens[j].delimiter);
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
                    char* command=events[index-1];
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
                            printf("%s",events[i]);   
                        }
                    }
                }
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
                write_past_events(events, event_count, history_file_path);
            }
        }

    }

    return 0;
}
