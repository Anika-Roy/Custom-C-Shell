#include "peek.h"

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

void print_file_details(const char *path)
{ // referred from ChatGPT
    struct stat file_stat;
    if (stat(path, &file_stat) == 0)
    {
        // Print the permissions
        // Print for user
        printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
        printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
        printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
        // group
        printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
        printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
        // and others permissions
        printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
        printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");

        struct passwd *owner_info = getpwuid(file_stat.st_uid);
        struct group *group_info = getgrgid(file_stat.st_gid);
        printf(" %s %s", owner_info->pw_name, group_info->gr_name);

        printf(" %ld", file_stat.st_size);

        struct tm *mod_time = localtime(&file_stat.st_mtime);
        printf(" %d-%02d-%02d %02d:%02d\n", mod_time->tm_year + 1900,
               mod_time->tm_mon + 1, mod_time->tm_mday,
               mod_time->tm_hour, mod_time->tm_min);
    }
}

void peek(char *flags[], int flag_count, char *store_previous_directory, char *store_calling_directory)
{
    char *path_or_name = flags[flag_count - 1];

    // check if the last argument is a flag or a path
    if (strcmp(path_or_name, "-l") == 0 || strcmp(path_or_name, "-a") == 0 || strcmp(path_or_name, "-al") == 0 || strcmp(path_or_name, "-la") == 0 || strcmp(path_or_name, "peek") == 0)
    {
        // then path or name is current directory
        path_or_name = store_calling_directory;
    }
    {
        // then path or name is current directory
        path_or_name = store_calling_directory;
    }

    // check if the last argument is '-'
    if (strcmp(path_or_name, "-") == 0)
    {
        // then path or name is previous directory
        path_or_name = store_previous_directory;
    }

    int show_hidden = 0;
    int show_details = 0;

    // Parse the flags
    for (int i = 1; i < flag_count; i++)
    {
        if (strcmp(flags[i], "-a") == 0)
        {
            show_hidden = 1;
        }
        else if (strcmp(flags[i], "-l") == 0)
        {
            show_details = 1;
        }
        else if (strcmp(flags[i], "-al") == 0 || strcmp(flags[i], "-la") == 0)
        {
            show_details = 1;
            show_hidden = 1;
        }
    }

    // Open the directory
    DIR *d;
    struct dirent *dir;

    d = opendir(path_or_name);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (show_hidden || dir->d_name[0] != '.')
            {
                // Print with different colors based on type
                struct stat file_stat;
                if (stat(dir->d_name, &file_stat) == 0)
                {
                    const char *color_code = ANSI_COLOR_RESET;
                    //check for directory
                    if (S_ISDIR(file_stat.st_mode)) {
                        color_code = ANSI_COLOR_BLUE;
                    } else if (file_stat.st_mode & S_IXUSR) {
                        color_code = ANSI_COLOR_GREEN;
                    } else {
                        color_code = ANSI_COLOR_WHITE;
                    }
 
                    if (show_details)
                    {
                        printf("%s%s%s ", color_code, dir->d_name, ANSI_COLOR_RESET);
                        print_file_details(path_or_name);
                    }
                    else 
                        printf("%s%s%s\n", color_code, dir->d_name, ANSI_COLOR_RESET);
                }
            }
        }
        closedir(d);
    }
}