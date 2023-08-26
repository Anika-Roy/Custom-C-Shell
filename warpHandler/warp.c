#include "../headers.h"
#include "warp.h"
#define MAX_LENGTH 1024

void warp(char *args[], int count, char *store_calling_directory, char* previous_directory) {
    /*
    Github copilot gave the basic structure and auto-completed error handling
    Things like printing working directory and changing previous directory were done by me
    */

    // printf("%s\n", store_calling_directory);
    if(count>1 && strcmp(args[1],"-")!=0 || count==1){
        // store the current directory as the previous directory
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }
        strcpy(previous_directory, cwd);
    }

    // printf("%s\n", store_calling_directory);

    // If no arguments are present, warp into the home directory 
    if (count == 1) {
        if (chdir(store_calling_directory) != 0) {
            perror("chdir");
            exit(EXIT_FAILURE);
        }
    }
    else {
        // If more than one argument is present, execute warp sequentially with all of them being the argument one by one (from left to right)
        for (int i = 1; i < count; i++) {
            
            // If the first letter of the argument is ~, replace with the home directory
            if (args[i][0] == '~') {
                char new_path[MAX_LENGTH];
                new_path[0] = '\0';// empty string
                strcpy(new_path, store_calling_directory);
                strcat(new_path, args[i] + 1);
                if (chdir(new_path) != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
            }
            // If the argument is -, warp into the previous directory
            else if (strcmp(args[i], "-") == 0) {
                if (chdir(previous_directory) != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
            }
            // If the argument is ., do nothing
            else if (strcmp(args[i], ".") == 0) {
                continue;
            }
            // If the argument is .., warp into the parent directory
            else if (strcmp(args[i], "..") == 0) {
                if (chdir("..") != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
            }
            // If the argument is neither of the above, warp into the mentioned directory
            else {
                if (chdir(args[i]) != 0) {
                    perror("chdir");
                    exit(EXIT_FAILURE);
                }
            }
  
        }
    }
    // Get the current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
    // Print the current working directory
    printf("%s\n", cwd);
    return;

}