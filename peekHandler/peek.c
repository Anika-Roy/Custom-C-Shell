# include "peek.h"

/*
Specification 4 : peek [8]
‘peek’ command lists all the files and directories in the specified directories in lexicographic order (default peek does not show hidden files). You should support the -a and -l flags.

-l : displays extra information
-a : displays all files, including hidden files
Similar to warp, you are expected to support “.”, “..”, “~”, and “-” symbols.

Support both relative and absolute paths.
If no argument is given, you should peek at the current working directory.
Multiple arguments will not be given as input.
The input will always be in the format :

peek <flags> <path/name>
Handle the following cases also in case of flags :

peek -a <path/name>
peek -l <path/name>
peek -a -l <path/name>
peek -l -a <path/name>
peek -la <path/name>
peek -al <path/name>
Note :

You can assume that the paths/names will not contain any whitespace characters.
DON’T use ‘execvp’ or similar commands for implementing this.
Use specific color coding to differentiate between file names, directories and executables in the output [green for executables, white for files and blue for directories].
Print a list of file/folders separated by newline characters.
The details printed with -l should be the same as the ls command present in Bash.

*/

void peek(char* flags[], int flag_count, char* store_previous_directory){

    // check no of arguements sent
    // if no flags are sent, show default peek behaviour
    
    if(flag_count==1){
        // peek at directory specified by the path/name
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                if(dir->d_name[0]!='.'){
                    printf("%s\n", dir->d_name);
                }
            }
            closedir(d);
        }
        return;
    }

    char* path_or_name=flags[1];
    
    int show_hidden = 0;
    int show_details = 0;

    // Parse the flags
    for (int i = 1; i < flag_count - 1; i++) {
        if (strcmp(flags[i], "-a") == 0) {
            show_hidden = 1;
        } else if (strcmp(flags[i], "-l") == 0) {
            show_details = 1;
        }
        else if(strcmp(flags[i], "-al") == 0 || strcmp(flags[i], "-la") == 0){
            show_details=1;
            show_hidden=1;
        }
    }

    // Open the directory
    DIR *d;
    struct dirent *dir;
    d = opendir(path_or_name);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (show_hidden || (dir->d_name[0] != '.')) {
                // Print with different colors based on type
                if (show_details) {
                    printf("%s\n", dir->d_name); // Extra information
                } else {
                    printf("%s\n", dir->d_name); // Just names
                }
            }
        }
        closedir(d);
    }
}