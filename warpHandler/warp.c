#include "../headers.h"
#include "warp.h"
#define MAX_LENGTH 1024

/*
‘warp’ command changes the directory that the shell is currently in. It should also print the full path of working directory after changing. The directory path/name can be provided as argument to this command.

You are also expected to implement the “.”, “..”, “~”, and “-” flags in warp. ~ represents the home directory of shell (refer to specification 1).
You should support both absolute and relative paths, along with paths from home directory.
If more than one argument is present, execute warp sequentially with all of them being the argument one by one (from left to right).
If no argument is present, then warp into the home directory.
*/

void warp(char *args[], int count, char *store_calling_directory, char* previous_directory) {

    // printf("%s\n", store_calling_directory);
    if(count>1 && strcmp(args[1],"-")!=0){
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
            // printf("%d\n", i);
            // printf("%s\n", args[i]);
            // If the argument is ~, warp into the home directory
            if (strcmp(args[i], "~") == 0) {
                if (chdir(store_calling_directory) != 0) {
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
            // If the argument is neither of the above, warp into the directory
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