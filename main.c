#include "headers.h"
#include "promptHandler/prompt.h"
#include "warpHandler/warp.h"
#include "peekHandler/peek.h"
#include "pasteventsHandler/pastevents.h"
#include "seekHandler/seek.h"

// Data structure to store background processes using array
struct BackgroundProcess background_processes[MAX_PROCESSES];
int background_process_count = 0;

// insert a background process into an array
void insert_background_process(pid_t pid, char *name) {// autocompleted by Copilot and Skeleton given by ChatGPT
    // Insert the process into the array
    // Update background_process_count
    if (background_process_count < MAX_PROCESSES) {
        struct BackgroundProcess process;
        process.pid = pid;
        strcpy(process.name, name);
        strcpy(process.status, "Running"); // Initialize status as running
        
        background_processes[background_process_count] = process;
        background_process_count++;
    } else {
        fprintf(stderr, "Maximum number of background processes reached\n");
    }
}

// remove a background process from an array
void remove_background_process(int index) {
    if (index >= 0 && index < background_process_count) {
        for (int i = index; i < background_process_count - 1; i++) {
            background_processes[i] = background_processes[i + 1];
        }
        background_process_count--;
    }
}
void check_background_processes_async() {
    for (int i = 0; i < background_process_count; i++) {
        pid_t pid = background_processes[i].pid;
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);

        // printf("result: %d\n",result);
        if (result == -1) {
            // An error occurred
            perror("waitpid");
        } else if (result == 0) {
            // The process is still running
            continue;
        } else {
            // The process has exited
            if (WIFEXITED(status)) {
                // The process exited normally
                // printf("%s (PID %d) exited normally with status %d\n", background_processes[i].name, pid, WEXITSTATUS(status));
                // Update the status in your data structure
                strcpy(background_processes[i].status, "Finished");
                remove_background_process(i);
                i--;
            } else if (WIFSIGNALED(status)) {
                // The process was terminated by a signal
                // printf("%s (PID %d) terminated abnormally by signal %d\n", background_processes[i].name, pid, WTERMSIG(status));
                // Update the status in your data structure
                strcpy(background_processes[i].status, "Failed");
                remove_background_process(i);
                i--;
            }
        }
    }
}

void check_background_processes_sync() {
    for (int i = 0; i < background_process_count; i++) {
        pid_t pid = background_processes[i].pid;
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);

        // printf("result: %d\n",result);
        if (result == -1) {
            // An error occurred
            // perror("waitpid");
        } else if (result == 0) {
            // The process is still running
            continue;
        } else {
            // The process has exited
            if (WIFEXITED(status)) {
                // The process exited normally
                // printf("%s (PID %d) exited normally with status %d\n", background_processes[i].name, pid, WEXITSTATUS(status));
                // Update the status in your data structure
                strcpy(background_processes[i].status, "Finished");
                // remove_background_process(i);
                // i--;
            } else if (WIFSIGNALED(status)) {
                // The process was terminated by a signal
                printf("%s (PID %d) terminated abnormally by signal %d\n", background_processes[i].name, pid, WTERMSIG(status));
                // Update the status in your data structure
                strcpy(background_processes[i].status, "Failed");
                // remove_background_process(i);
                // i--;
            }
        }
    }
}


void print_background_processes(){
    for (int i = 0; i < background_process_count; i++) {
        if(strcmp(background_processes[i].status,"Running")!=0){
            if(strcmp(background_processes[i].status,"Finished")==0)
                printf("%s (PID %d) exited normally\n", background_processes[i].name, background_processes[i].pid);
            else if(strcmp(background_processes[i].status,"Failed")==0)
                printf("%s (PID %d) terminated abnormally\n", background_processes[i].name, background_processes[i].pid);
            
            remove_background_process(i);
            i--;
        }
    }
}

void execute_background(char *args[]) {
    // taken from ChatGPT
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // Child process
        // Create a new session and detach from terminal
        if (setsid() == -1) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }

        // Redirect standard input, output, and error to /dev/null
        int dev_null = open("/dev/null", O_RDWR);
        if (dev_null == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        dup2(dev_null, STDIN_FILENO);
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);
        close(dev_null);

        // Execute the command using execvp
        execvp(args[0], args);
        perror("execvp"); // This will be executed only if execvp fails
        exit(EXIT_FAILURE);
    } else {
        // Parent process

        // Store child_pid in your data structure for background processes
        insert_background_process(child_pid, args[0]);

        printf("[%d] %d\n", background_process_count, child_pid);
        
    }
}

void handle_signal(int signum) {
    if (signum == SIGCHLD) {
        check_background_processes_sync();
        // check_background_processes_async();
    }
}

int main()
{
    // Set up signal handler for SIGCHLD
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    // Keep accepting commands
    char store_calling_directory[1024];
    getcwd(store_calling_directory, sizeof(store_calling_directory));

    // store the previous directory for warp
    char store_previous_directory[1024];
    getcwd(store_previous_directory, sizeof(store_previous_directory));

    // get the absolute path for the history file <FILENAME> and store it
    char history_file_path[1024];
    strcpy(history_file_path, store_calling_directory);
    strcat(history_file_path, "/");
    strcat(history_file_path, FILENAME);

    // Read past events from history_file_path
    char events[MAX_EVENTS][MAX_EVENT_LENGTH];
    int event_count=0;
    read_past_events(events, &event_count, history_file_path);   

    // print events for debugging
    // for(int i=0;i<event_count;i++){
    //     printf("%d->%s\n",i,events[i]);
    // }

    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(store_calling_directory,0);
        char input[4096];
        fgets(input, 4096, stdin);

        // Dclare a variable to store the original command, which we'll get by concatenating all tokens
        char original_command[MAX_EVENT_LENGTH];
        original_command[0]='\0';

       // declare array of structs to store tokens and their delimiters
        struct TokenWithDelimiter tokens[MAX_TOKENS];
        
        int i=tokeniser(tokens,input);

        int flag=1;
        int execute=0;
        int token_count=0;

        char delimiter;
        char *args[MAX_ARGS];
        int arg_count = 0;
        // execute all tokens in a loop (autocompleted by Copilot)
        for (int j = 0; j < i; j++) {   
            // get delimiter
            delimiter=tokens[j].delimiter;

            char command[MAX_COMMAND_LENGTH];

            // Tokenize with all whitespaces (space and tab) to get command and arguments
            if(execute==0){
                arg_count = 0;

                args[arg_count] = strtok(tokens[j].token, " \t\n");
                while (args[arg_count] != NULL && arg_count < MAX_ARGS) {
                    arg_count++;
                    args[arg_count] = strtok(NULL, " \t\n");
                }   
            }
            execute=0;

            // print all tokens and args for debugging
            // printf("Delimiter: %c\n", tokens[j].delimiter);
            // printf("Command: %s\n", args[0]);
            // printf("Arguments:\n");
            // for (int k = 1; k < arg_count; k++) {
            //     printf("%s\n", args[k]);
            // }

            // Concatenate all tokens(and their arguments) to get the original command(along with delimiter)
            if(strcmp(args[0],"pastevents")!=0){
                strcat(original_command, args[0]);
                for (int k = 1; k < arg_count; k++) {
                    strcat(original_command, " ");
                    strcat(original_command, args[k]);
                }
                if(token_count<i-1)
                    strcat(original_command, &delimiter);
                token_count++;
            }
            // If pastevents was present, check if second arg was execute, only then save it
            if((strcmp(args[0],"pastevents")==0 && arg_count>1 && strcmp(args[1],"execute")!=0) || (strcmp(args[0],"pastevents")==0 && arg_count==1) ){
                flag=0;
            }

            // If the delimiter is '&', execute the command in the background
            if (delimiter == '&') {
                execute_background(args);
                continue;
            }

            // If the delimiter is ';', execute the command in the foreground
            pid_t child_pid = fork();

            if (child_pid < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (child_pid == 0) {
                // Child process
                
                //---------------------------------------------------------------------------------

                // Now you have the command and its arguments in the args array
                if(strcmp(args[0],"exit")==0){
                    exit(0);
                }

                // If the command is warp, call the warp function
                else if (strcmp(args[0], "warp") == 0) {
                    warp(args, arg_count, store_calling_directory, store_previous_directory);
                }

                // If the command is peek, call the peek function
                else if (strcmp(args[0], "peek") == 0) {
                    peek(args, arg_count, store_previous_directory,store_calling_directory);
                }

                // If the command is seek, call the seek function
                else if (strcmp(args[0], "seek") == 0) {
                    seek(args, arg_count,store_calling_directory);
                }

                else if(strcmp(args[0],"pastevents")==0){
                    /*
                    if its pastevents execute <index>, 
                    replace the command in the args array with the command at index <index>, 
                    decrement j and run the loop again
                    This way, we don't store it in the history and we can execute it
                    */ 
                    if(arg_count>1 && strcmp(args[1],"execute")==0){
                        // args[2]="<index>"
                        int index=atoi(args[2]);
                        if(index>event_count){
                            printf("Index out of bounds\n");
                            continue;
                        }
                        strcpy(command,events[index-1]);
                        // printf("%s\n",command);
                        char* command_args[100];
                        int command_arg_count=0;
                        char* token=strtok(command," ");
                        while(token!=NULL){
                            command_args[command_arg_count]=token;
                            command_arg_count++;
                            token=strtok(NULL," ");
                        }
                        // printf("%s\n",command_args[0]);
                        // replace 
                        strcpy(args[0],command_args[0]);
                        for(int k=1;k<command_arg_count;k++){
                            strcpy(args[k],command_args[k]);
                        }
                        arg_count=command_arg_count;
                        j--;
                        // printf("args0:%s\n",args[0]);
                        execute=1;
                        continue;
                    }
                    else if(arg_count>1 && strcmp(args[1],"purge")==0){
                        event_count=0;
                        write_past_events(events,event_count,history_file_path);
                        continue;
                    }
                    else {
                        if(arg_count==1){
                            for(int i=0;i<event_count;i++){
                                printf("%s\n",events[i]);   
                            }
                        }
                    }
                }

                else{
                    // execute using execvp
                    int error_flag = execvp(args[0], args);
                    // if error occurs, print error
                    if (error_flag == -1) {
                        printf("ERROR : '%s' is not a valid command\n",args[0]);
                    }
                }

                //---------------------------------------------------------------------------------
            } else {
                // Parent process
                time_t start_time = time(NULL);
                int status;
                wait(&status);
                time_t end_time = time(NULL);
                
                if (end_time - start_time > 2) {
                    printf("Foreground process '%s' took %lds\n", args[0], (long)(end_time - start_time));
                    prompt(store_calling_directory,(long)(end_time - start_time));
                }
            }
            
        }   
        // if not, add the input to the list of past events
        // also, if the original command contains the work 'pastevents'(apart from ), don't add it to the list
        if (flag) {

            if (event_count == 0 || strcmp(events[event_count - 1], original_command) != 0) {
                add_event(original_command, events, &event_count);
                write_past_events(events, event_count, history_file_path);
            }
        }

        // Check if any background processes have completed
        check_background_processes_sync();
        print_background_processes();
    }

    return 0;
}
