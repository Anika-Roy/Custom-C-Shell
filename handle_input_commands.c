#include "headers.h"
#include "handle_input_commands.h"

/*
Specification2
These cases must be handled:
1. Your shell should support a ‘;’ or ‘&’ separated list of commands. You can use ‘strtok’ to tokenize the input.
2. Your shell should account for random spaces and tabs when taking input.
3. The “;” command can be used to give multiple commands at the same time. This works similar to how “;” works in Bash.
4. ‘&’ operator runs the command preceding it in the background after printing the process id of the newly created process.
*/

/*
ChatGPT usage:
1. used for implementing point 3 and 4
*/

void print_tokens(char *tokens[], int i)
{
    for (int j = 0; j < i; j++)
    {
        printf("%s\n", tokens[j]);
    }
}

// Function to trim whitespace from a string -->   ChatGPT referred
char* trim_whitespace(char *str)
{
    // Remove leading spaces and tabs
    while (*str == ' ' || *str == '\t')
    {
        str++;
    }

    // Find the end of the string
    char *end = str + strlen(str) - 1;

    // Remove trailing spaces and tabs
    while (end > str && (*end == ' ' || *end == '\t'))
    {
        end--;
    }

    // Null-terminate the trimmed string
    *(end + 1) = '\0';

    return str;
}

// Function to execute a command -->   ChatGPT referred
void execute_seq_command(char* token){
    // Create a new process using fork
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // Child process
    {
        // Tokenize the command for exec
        char *command = token;
        char *args[64]; // Adjust the size if needed

        char *token = strtok(command, " ");
        int arg_index = 0;
        while (token != NULL)
        {
            args[arg_index++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_index] = NULL;

        // Execute the command using exec
        execvp(args[0], args);
        
        // If execvp returns, an error occurred
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else // Parent process
    {
        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
        
        // Check if the child process exited normally
        if (WIFEXITED(status))
        {
            printf("Command exited normally\n");
        }
    }
}

// Function to execute background commands -->   ChatGPT referred
void execute_background_commands(char* tokens[], int i){
    // Create a new process using fork
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // Child process
    {
        // Tokenize the command for exec
        char *command = tokens[0];
        char *args[64]; // Adjust the size if needed

        char *token = strtok(command, " ");
        int arg_index = 0;
        while (token != NULL)
        {
            args[arg_index++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_index] = NULL;

        // Execute the command using exec
        execvp(args[0], args);
        
        // If execvp returns, an error occurred
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else // Parent process
    {
        // Print the process id of the child process
        printf("Process id of the child process: %d\n", pid);
    }
}

void handle_input_commands(char* input){

    //implement Specification2
    // point 1 and store these tokens for later use
    char *token = strtok(input, ";");
    //store tokens
    char *tokens[1024];
    int i = 0;
    while (token != NULL)
    {
        tokens[i] = token;
        token = strtok(NULL, ";");
        i++;
    }

    //print the list of tokens for debugging
    print_tokens(tokens, i);

    // point 2 - handle whitespaces and tabs
    for (int j = 0; j < i; j++)
    {
        tokens[j]=trim_whitespace(tokens[j]);
    }

    print_tokens(tokens, i);

    // point 3 and 4
    // implement sequential and background commands

    char* tokens_with_background[1024];

    // Process each token to check for background execution
    for (int j = 0; j < i; j++)
    {
        // Check for background execution
        char *background_token = strstr(tokens[j], "&");

        if(background_token!=NULL){
            // tokenise the token removing the '&' character
            int i_with_background = 0;
            while (background_token != NULL)
            {
                tokens_with_background[i_with_background] = background_token;
                token = strtok(NULL, "&");
                i_with_background++;
            }

            // Trim leading and trailing whitespace from the token
            for (int k = 0; k < i_with_background; k++)
            {
                tokens_with_background[j]=trim_whitespace(tokens_with_background[j]);
            }
            // execute the background commands
            execute_background_commands(tokens_with_background, i_with_background);
            
        }

        else{
            // execute the sequential command
            execute_seq_command(tokens[j]);
        }

    }
    





}