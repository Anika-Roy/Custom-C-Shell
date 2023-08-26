#ifndef SEEK_H_
#define SEEK_H_

# include "../headers.h"
# include <dirent.h>
#include <sys/stat.h>
void seek(char* args[], int arg_count, char *store_calling_directory);
void seek_file(const char *search, const char *target_dir,int* file_count,int target_length);
void seek_directory(const char *search, const char *target_dir, int* dir_count,int target_length);

#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_WHITE   "\x1b[37m"


#endif