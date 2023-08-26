#include "seek.h"

void seek_file(const char *search, const char *target_dir,int* file_count) {
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
            seek_file(search, sub_dir_path,file_count); // Recursively search subdirectories
        }  else if (entry->d_type == DT_REG && strcmp(entry->d_name, search) == 0) {
            printf("%s/%s\n", target_dir, entry->d_name);
            (*file_count)++;
        }
    }
    closedir(dir);
}

void seek_directory(const char *search, const char *target_dir,int* dir_count) {
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
                (*dir_count)++;
            }
            seek_directory(search, sub_dir_path,dir_count); // Recursively search subdirectories
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

    // if -d is 1, search for directory
    if(d==1){
        seek_directory(search,target_dir,&dir_count);
    }
    // if -f is 1, search for file
    else if(f==1){
        seek_file(search,target_dir,&file_count);
    }
    // if flag count is 0, search for both
    else if(flag_count==0 || e==1){
        seek_directory(search,target_dir,&dir_count);
        seek_file(search,target_dir,&file_count);
    }
    if(e==1){
        /*
        -e : This flag is effective only when a single file or a single directory with the name is found. 
        If only one file (and no directories) is found, then print it’s output. If only one directory 
        (and no files) is found, then change current working directory to it. Otherwise, the flag has no 
        effect. This flag should work with -d and -f flags. If -e flag is enabled but the directory does 
        not have access permission (execute) or file does not have read permission, then output 
        “Missing permissions for task!”
        */
        if(dir_count==1 && file_count==0){
            // change current working directory to it
            if(chdir(search)!=0){
                printf("Missing permissions for task!\n");
            }
        }
        else if(dir_count==0 && file_count==1){
            // print it's output
            // printf("%s\n",search);
            // print the contents of the file
            FILE *fp;
            char ch;
            fp = fopen(search,"r");
            if(fp==NULL){
                printf("Missing permissions for task!\n");
                return;
            }
            while((ch=fgetc(fp))!=EOF){
                printf("%c",ch);
            }
            fclose(fp);
        }
        else{
            // do nothing
            return;
        }

    }
    // No match found
    if(dir_count==0 && file_count==0){
        printf("No match found\n");
    }

    return;
}
