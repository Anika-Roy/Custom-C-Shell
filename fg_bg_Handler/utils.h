#ifndef UTILS_H
#define UTILS_H

#include "../headers.h"

void insert_background_process(pid_t pid, char *name,int* background_process_count,struct BackgroundProcess background_processes[]);
void remove_background_process(int index,int* background_process_count,struct BackgroundProcess background_processes[]);
void check_background_processes_sync(int* background_process_count,struct BackgroundProcess background_processes[]);
void print_background_processes(int* background_process_count,struct BackgroundProcess background_processes[]);

#endif