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

void handle_input_commands(char* input){

    //implement Specification2
    // point 1 and store these tokens for later use
    char *token = strtok(input, ";&");
    //store tokens
    char *tokens[1024];
    int i = 0;
    while (token != NULL)
    {
        tokens[i] = token;
        token = strtok(NULL, ";&");
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

    // point 3

    
    // point 4
    





}