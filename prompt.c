#include "headers.h"
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
void prompt(char *store_calling_directory) {
    /* 
    ChatGPT and Copilot usage: error handling and import libraries
    */
    char hostname[1024];
    char cwd[1024];

    // Get the username
    char *username = getenv("USER");
    if (username == NULL) {
        perror("getenv");
        exit(EXIT_FAILURE);
    }
    
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

    if (strstr(cwd, store_calling_directory) == cwd) {
        strcpy(cwd, "~");
    }

    printf("<%s@%s:%s> ", username, hostname, cwd);
}
    

