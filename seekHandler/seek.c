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
        if (entry->d_type == DT_REG && strcmp(entry->d_name, search) == 0) {
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
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, search) == 0) {
            printf("%s/%s/\n", target_dir, entry->d_name);
        }
    }
    closedir(dir);
}

void seek(char* args, int arg_count) {
    // obtained from ChatGPT
    if (arg_count < 3 || arg_count > 4) {
        printf("Usage: %s <flags> <search> [<target_directory>]\n", args[0]);
        return 1;
    }

    int flags = 0; // 0: no flag, 1: -d, 2: -f, 3: -e
    char *search = args[2];
    char *target_dir = (arg_count == 4) ? args[3] : ".";

    if (arg_count == 4 && access(target_dir, F_OK) == -1) {
        printf("Target directory does not exist\n");
        return 1;
    }

    for (int i = 1; i < strlen(args[1]); i++) {
        if (args[1][i] == 'd') {
            flags |= 1;
        } else if (args[1][i] == 'f') {
            flags |= 2;
        } else if (args[1][i] == 'e') {
            flags |= 3;
        } else {
            printf("Invalid flag: %c\n", args[1][i]);
            return ;
        }
    }

    if ((flags & 1) && (flags & 2)) {
        printf("Invalid flags!\n");
        return 1;
    }

    int file_count = 0;
    int dir_count = 0;

    DIR *dir = opendir(target_dir);
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if ((flags & 1) && entry->d_type != DT_DIR) {
            continue;
        }
        if ((flags & 2) && entry->d_type != DT_REG) {
            continue;
        }
        if (strcmp(entry->d_name, search) == 0) {
            if (entry->d_type == DT_REG) {
                file_count++;
            } else if (entry->d_type == DT_DIR) {
                dir_count++;
            }
        }
    }
    closedir(dir);

    if (dir_count + file_count == 1) {
        if (flags & 3) {
            if (file_count == 1) {
                char full_path[1024];
                snprintf(full_path, sizeof(full_path), "%s/%s", target_dir, search);
                FILE *file = fopen(full_path, "r");
                if (file) {
                    char line[1024];
                    while (fgets(line, sizeof(line), file)) {
                        printf("%s", line);
                    }
                    fclose(file);
                } else {
                    printf("Missing permissions for task!\n");
                }
            } else if (dir_count == 1) {
                chdir(target_dir);
            }
        } else {
            seek_file(search, target_dir);
            seek_directory(search, target_dir);
        }
    } else if (dir_count + file_count > 1) {
        seek_file(search, target_dir);
        seek_directory(search, target_dir);
    } else {
        printf("No match found!\n");
    }

    return;
}
