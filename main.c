#include "headers.h"
#include "promptHandler/prompt.h"
#include "warpHandler/warp.h"
#include "peekHandler/peek.h"

#define MAX_TOKENS 1024
#define MAX_ARGS 64

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

    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(store_calling_directory);
        char input[4096];
        fgets(input, 4096, stdin);
        

        // tokenise the input --> store in tokens as structs (autocompleted by Copilot)
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
            // printf("Command: %s\n", args[0]);
            // printf("Arguments: \n");
            // for (int k = 1; k < arg_count; k++) {
            //     printf("%s ", args[k]);
            // }
            // printf("\n");

            // Now you have the command and its arguments in the args array
            if(strcmp(args[0],"exit")==0){
                exit(0);
            }

            // If the command is warp, call the warp function
            if (strcmp(args[0], "warp") == 0) {
                //check for delimiter[TODO]
                warp(args, arg_count, store_calling_directory, store_previous_directory);
            }

            // If the command is peek, call the peek function
            if (strcmp(args[0], "peek") == 0) {
                //check for delimiter[TODO]
                peek(args, arg_count, store_previous_directory);
            }
            
        }

        



        


    }
}
