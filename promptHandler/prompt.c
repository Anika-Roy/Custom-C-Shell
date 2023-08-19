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
    

