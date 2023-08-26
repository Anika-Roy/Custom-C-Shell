#ifndef SEEK_H_
#define SEEK_H_

# include "../headers.h"
# include <dirent.h>
#include <sys/stat.h>
void seek(char* args[], int arg_count, char *store_calling_directory);
void seek_file(const char *search, const char *target_dir);
void seek_directory(const char *search, const char *target_dir);


#endif