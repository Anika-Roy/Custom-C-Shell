#include "peek.h"

/*
Specification 4 : peek [8]
‘peek’ command lists all the files and directories in the specified directories in lexicographic order (default peek does not show hidden files). 
You should support the -a and -l flags.

Github Copilot and ChatGPT usage overview:
*/

void file_handler_function(char path_or_name[],char *store_previous_directory, char *store_calling_directory){
    // check if the last argument is a flag or a path
    if (strcmp(path_or_name, "-l") == 0 || strcmp(path_or_name, "-a") == 0 || strcmp(path_or_name, "-al") == 0 || strcmp(path_or_name, "-la") == 0 || strcmp(path_or_name, "peek") == 0){
        // then path or name is current directory
        // Get the current working directory
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }
        strcpy(path_or_name,cwd);
    }

    // If the first letter of the argument is ~, replace with the home directory
    if (path_or_name[0] == '~') {
        char new_path[MAX_PATH_LENGTH];
        new_path[0] = '\0';// empty string
        strcpy(new_path, store_calling_directory);
        strcat(new_path, path_or_name + 1); // add the rest of the path (without the ~)

        strcpy(path_or_name, new_path);// copy the new path back to the original string 
    }
    // check if the last argument is '-'
    if (strcmp(path_or_name, "-") == 0){
        // then path or name is previous directory
        strcpy(path_or_name,store_previous_directory);
    }
}

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

int peek(char *flags[], int flag_count, char *store_previous_directory, char *store_calling_directory)
{
    char path_or_name[MAX_PATH_LENGTH];
    strcpy(path_or_name,flags[flag_count - 1]);

    // call filehandler
    file_handler_function(path_or_name,store_previous_directory, store_calling_directory);
    // printf("%s\n",path_or_name);

    int show_hidden = 0;
    int show_details = 0;

    // Parse the flags -> referred from ChatGPT
    for (int i = 1; i < flag_count; i++){
        if (strcmp(flags[i], "-a") == 0)
            show_hidden = 1;
        else if (strcmp(flags[i], "-l") == 0)
            show_details = 1;
        else if (strcmp(flags[i], "-al") == 0 || strcmp(flags[i], "-la") == 0){
            show_details = 1;
            show_hidden = 1;
        }
    }

    // Open the directory -> referred from ChatGPT + Copilot
    // For lexicographic order, prompted ChatGPT to sort using alphasort after asking others
    DIR *d;
    struct dirent *dir;

    struct dirent **entries;
    int num_entries;

    d = opendir(path_or_name);
    if (d)
    {
        num_entries = scandir(path_or_name, &entries, NULL, alphasort);
        if (num_entries < 0) {
            perror("scandir");
            return 1;
        }
        for (int i = 0; i < num_entries; i++) {
            dir = entries[i];
            // printf("%s\n", dir->d_name);
            if (show_hidden || dir->d_name[0] != '.')
            {
                // Print with different colors based on type
                struct stat file_stat;
                char full_path[MAX_PATH_LENGTH];
                snprintf(full_path, sizeof(full_path), "%s/%s", path_or_name, dir->d_name);

                // printf("stat: %d\n",stat(dir->d_name, &file_stat));

                if(stat(full_path, &file_stat) == 0){
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
                        print_file_details(full_path);
                    }
                    else 
                        printf("%s%s%s\n", color_code, dir->d_name, ANSI_COLOR_RESET);
                }
            }
            free(entries[i]);
        }
        free(entries);
        closedir(d);
    }
    return 0;
}