#include "seek.h"

void seek_file(const char *search, const char *target_dir) {
    // obtained from ChatGPT
    DIR *dir = opendir(target_dir);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char sub_dir_path[1024];
            snprintf(sub_dir_path, sizeof(sub_dir_path), "%s/%s", target_dir, entry->d_name);
            seek_file(search, sub_dir_path); // Recursively search subdirectories
        } else if (entry->d_type == DT_REG && strcmp(entry->d_name, search) == 0) {
            printf("%s/%s\n", target_dir, entry->d_name);
        }
    }
    closedir(dir);
}

void seek_directory(const char *search, const char *target_dir) {
    // obtained from ChatGPT
    DIR *dir = opendir(target_dir);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char sub_dir_path[1024];
            snprintf(sub_dir_path, sizeof(sub_dir_path), "%s/%s", target_dir, entry->d_name);
            if (strcmp(entry->d_name, search) == 0) {
                printf("%s/\n", sub_dir_path);
            }
            seek_directory(search, sub_dir_path); // Recursively search subdirectories
        }
    }
    closedir(dir);
}

void seek(char* args[], int arg_count, char *store_calling_directory) {
    // obtained from ChatGPT
    
    // Handle flags -- iterate through the flags and check if -d, -f or -e exist
    // the next argument after flags would be search

    int d=0,f=0,e=0;
    for(int i=0;i<arg_count;i++){
        if(strcmp(args[i],"-d")==0)
            d=1;
        else if(strcmp(args[i],"-f")==0)
            f=1;
        else if(strcmp(args[i],"-e")==0)
            e=1;
    }

    // if -d and -f are both one, print error
    if(d==1 && f==1){
        printf("Invalid flags\n");
        return;
    }

    int flag_count=d+e+f;

    // get search and target_dir
    char *search = args[flag_count+1];

    // if no target directory is specified, search in current directory
    char *target_dir;
    if(flag_count+2==arg_count){
        target_dir=".";
    }
    else
        target_dir= args[flag_count+2];
    

    int file_count = 0;
    int dir_count = 0;

    DIR *dir = opendir(target_dir);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            file_count++;
        } else if (entry->d_type == DT_DIR) {
            dir_count++;
        }
    }
    closedir(dir);


    // if -d is 1, search for directory
    if(d==1){
        seek_directory(search,target_dir);
    }
    // if -f is 1, search for file
    else if(f==1){
        seek_file(search,target_dir);
    }
    // if -e is 1, search for both
    else if(e==1 || flag_count==0){
        seek_directory(search,target_dir);
        seek_file(search,target_dir);
    }

    return;
}
