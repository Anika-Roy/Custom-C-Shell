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
    // tokenise the input --> store in tokens as structs (autocompleted by Copilot)
    // reference from linux manpages
 
    // Tokenize with all whitespaces (space and tab) to get command and arguments
    char *str1, *str2, *token, *subtoken, *saveptr1, *saveptr2;
    int j = 0;
    for (str1 = input; ; str1 = NULL) {
        token = strtok_r(str1, ";", &saveptr1);
        if (token == NULL)
            break;

        // printf("%d: %s\n", i, token);

        for (str2 = token; ; str2 = NULL) {
            subtoken = strtok_r(str2, "&", &saveptr2);

            //print
            // printf(" --> %s\n", subtoken);
            
            // Check if the subtoken is the last token
            if (subtoken == NULL) {
                tokens[j-1].delimiter = ';';  // Set the delimiter directly
                break;
            }
            else{
                strcpy(tokens[j].token, subtoken);
                tokens[j].delimiter = '&';
            }
            j++;    
        }
    }
    // printf("%d\n",j);
    //Check if the last token is a newline:
    // if it is, decrement j by 1 and return
    if(isspace(tokens[j-1].token[0]))
        j--;

    // printf("%d\n",j);
    // print the tokens
    // for (int k = 0; k < j; k++) {
    //     printf("%d: %s\n", k, tokens[k].token);
    //     printf("%d: %c\n", k, tokens[k].delimiter);
    // }
    return j;
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
        printf("<%s@%s:~%s> ", username, hostname, relative_path + strlen(store_calling_directory));
    }
    else{
        printf("<%s@%s:%s> ", username, hostname, cwd);
    }

}
    

