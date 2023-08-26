#include "../headers.h"
#include "prompt.h"

/*
Specification1
On every line when waiting for user input, a shell prompt of the following form must appear along with it.
With proper error handling.
Format: <Username@SystemName:~>
Example: <JohnDoe@SYS:/home/johndoe/sem3>
Note: The directory from which shell is invoked becomes the home directory for the shell and 
represented with “~”. All paths inside this directory should be shown relative to it. 
Absolute path of a directory/file must be shown when outside the home directory.

*/
int tokeniser(struct TokenWithDelimiter tokens[], char input[]) {
    // After a lot of trial and error, discussed with Ujjwal Shekhar(2021113009)
 
    // Tokenize with all whitespaces (space and tab) to get command and arguments

    char input_copy[4096];
    strcpy(input_copy, input);
    
    // tokenise input copy with ; and & to get commands
    char *command = strtok(input_copy, ";&");
    int command_count = 0;
    while (command != NULL) {
        // printf("%s\n", command);
        strcpy(tokens[command_count].token, command);
        command_count++;
        command = strtok(NULL, ";&\n");
    }

    // check if the last token has only whitespaces
    // if yes, then decrement command_count
    int last_token_length = strlen(tokens[command_count - 1].token);
    int last_token_is_whitespace = 1;
    for (int i = 0; i < last_token_length; i++) {
        if (tokens[command_count - 1].token[i] != ' ' && tokens[command_count - 1].token[i] != '\t') {
            last_token_is_whitespace = 0;
            break;
        }
    }
    if (last_token_is_whitespace) {
        command_count--;
    }

    // now using the original input string, get the corresponding delimiter in tokens[i].delimiter
    // compare the tokens and skip whitespaces to find the delimiter
    // if the token is the last token, then the delimiter is ;
    int i = 0;
    int j = 0;
    tokens[command_count - 1].delimiter = ';';
    while (input[i] != '\0' && j < command_count) {
        if (input[i] == ' ' || input[i] == '\t') {
            i++;
            continue;
        }

        if (input[i] == ';' || input[i] == '&') {
            tokens[j].delimiter = input[i];
            j++;
        }

        i++;
    }

    //print the tokens and their delimiters
    // for (int i = 0; i < command_count; i++) {
    //     printf("token: %s\n", tokens[i].token);
    //     printf("delimiter:%c\n", tokens[i].delimiter);
    // }

    return command_count;
}

void prompt(char *store_calling_directory) {
    /* 
    ChatGPT and Copilot usage: error handling and import libraries
    */
    char hostname[1024];
    char cwd[1024];

    // Get the username (automatically imported by Copilot)
    struct passwd *p = getpwuid(getuid());
    char *username = p->pw_name;

    
    // Get the system name (hostname)
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }

    // Get the current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    //show the relative path from the home directory (autocompleted by Copilot)
    if (strstr(cwd, store_calling_directory) != NULL) {
        char *relative_path = strstr(cwd, store_calling_directory);
        // printf("%s\n", relative_path);
        printf("<%s@%s:~%s> ", username, hostname, relative_path + strlen(store_calling_directory));
    }
    else{
        printf("<%s@%s:%s> ", username, hostname, cwd);
    }

}
    

