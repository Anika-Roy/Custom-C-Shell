#include "../headers.h"

// insert a background process into an array
void insert_background_process(pid_t pid, char *name,int* background_process_count,struct BackgroundProcess background_processes[]) {
    // autocompleted by Copilot and Skeleton given by ChatGPT
    // Insert the process into the array
    // Update background_process_count
    if (*background_process_count < MAX_PROCESSES) {
        struct BackgroundProcess process;
        process.pid = pid;
        strcpy(process.name, name);
        strcpy(process.status, "Running"); // Initialize status as running
        
        background_processes[*background_process_count] = process;
        (*background_process_count)++;
    } else {
        fprintf(stderr, "Maximum number of background processes reached\n");
    }
}

// remove a background process from an array
void remove_background_process(int index,int* background_process_count,struct BackgroundProcess background_processes[]) {
    if (index >= 0 && index < *background_process_count) {
        for (int i = index; i < *background_process_count - 1; i++) {
            background_processes[i] = background_processes[i + 1];
        }
        (*background_process_count)--;
    }
    return;
}

void check_background_processes_sync(int* background_process_count,struct BackgroundProcess background_processes[]) {
    for (int i = 0; i < *background_process_count; i++) {
        pid_t pid = background_processes[i].pid;
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG | WUNTRACED);

        if (result == -1) {
            // An error occurred
            // perror("waitpid hello");
        } else if (result == 0) {
            // The process is still running
            continue;
        } else {
            // The process has exited
            if (WIFEXITED(status)) {
                // The process exited normally
                // Update the status in your data structure
                // printf("exited:%s\n",background_processes[i].name);
                strcpy(background_processes[i].status, "Finished");
            } else if (WIFSIGNALED(status)) {
                // The process was terminated by a signal
                // Update the status in your data structure
                // printf("signaled:%s\n",background_processes[i].name);
                strcpy(background_processes[i].status, "Failed");
            }
            else if(WIFSTOPPED(status)){
                // printf("stopped:%s\n",background_processes[i].name);
                strcpy(background_processes[i].status, "Stopped");
            }
        }
    }
}


void print_background_processes(int* background_process_count,struct BackgroundProcess background_processes[]){
    for (int i = 0; i < *background_process_count; i++) {
        if(strcmp(background_processes[i].status,"Running")!=0 && strcmp(background_processes[i].status,"Stopped")!=0){
            if(strcmp(background_processes[i].status,"Finished")==0)
                printf("%s (PID %d) exited normally\n", background_processes[i].name, background_processes[i].pid);
            else if(strcmp(background_processes[i].status,"Failed")==0)
                printf("%s (PID %d) terminated abnormally\n", background_processes[i].name, background_processes[i].pid);
            
            remove_background_process(i,background_process_count,background_processes);
            i--;
        }
    }
    return;
}