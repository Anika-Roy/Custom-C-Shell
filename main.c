#include "headers.h"
#include "promptHandler/prompt.h"
#include "warpHandler/warp.h"
#include "peekHandler/peek.h"
#include "pasteventsHandler/pastevents.h"
#include "seekHandler/seek.h"
#include "activitiesHandler/activities.h"
#include "pingHandler/ping.h"

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
    return;
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
                printf("exited:%s\n",background_processes[i].name);
                strcpy(background_processes[i].status, "Finished");
            } else if (WIFSIGNALED(status)) {
                // The process was terminated by a signal
                // Update the status in your data structure
                printf("signaled:%s\n",background_processes[i].name);
                strcpy(background_processes[i].status, "Failed/Stopped");
            }
            else if(WIFSTOPPED(status)){
                printf("stopped:%s\n",background_processes[i].name);
                strcpy(background_processes[i].status, "Stopped");
            }
        }
    }
}


void print_background_processes(){
    for (int i = 0; i < background_process_count; i++) {
        if(strcmp(background_processes[i].status,"Running")!=0){
            if(strcmp(background_processes[i].status,"Finished")==0)
                printf("%s (PID %d) exited normally\n", background_processes[i].name, background_processes[i].pid);
            else if(strcmp(background_processes[i].status,"Failed/Stopped")==0)
                printf("%s (PID %d) terminated abnormally\n", background_processes[i].name, background_processes[i].pid);
            
            remove_background_process(i);
            i--;
        }
    }
    return;
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

        // concatenate the args to get the whole command
        char command[MAX_COMMAND_LENGTH];
        command[0]='\0';
        for(int i=0;args[i]!=NULL;i++){
            strcat(command,args[i]);
            if(args[i+1]!=NULL)
                strcat(command," ");
        }
        insert_background_process(child_pid, command);

        printf("[%d] %d\n", background_process_count, child_pid);
        
    }
}

void handle_signal(int signum) {
    switch (signum) {
        case SIGCHLD:
            printf("SIGCHLD received\n");
            // Handle child process terminated (SIGCHLD)
            // Add your handling code here
            check_background_processes_sync();
            break;
        case SIGTSTP:
            // Handle Ctrl+Z (SIGTSTP) - Suspend a process
            // Add your handling code here
            break;
        // Add cases for other signals you want to handle
        default:
            // Handle other signals as needed
            break;
    }
}

int main()
{
    // Set up signal handler for SIGCHLD
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    // Set up signal handlers for specific signals
    sigaction(SIGCHLD, &sa, NULL); // Handle child process terminated (SIGCHLD)
    // sigaction(SIGTSTP, &sa, NULL); // Handle Ctrl+Z (SIGTSTP)

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

    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        // printf("background_process_count: %d\n",background_process_count );
        prompt(store_calling_directory);
        char input[4096];
        fgets(input, 4096, stdin);

        //continue if input is empty
        if (strcmp(input, "\n") == 0)
            continue;

        // Handling PASTEVENTS EXECUTE: declare array of structs to store tokens and their delimiters 
        struct TokenWithDelimiter tokens_pastevents[MAX_TOKENS];
        char temp_input[4096];
        strcpy(temp_input,input);

        int i=tokeniser(tokens_pastevents,temp_input);

        // remove the newline character from the last token if present
        if(tokens_pastevents[i-1].token[strlen(tokens_pastevents[i-1].token)-1]=='\n')
            tokens_pastevents[i-1].token[strlen(tokens_pastevents[i-1].token)-1]='\0';

        // handle pastevents execute command by iterating through the tokens
        for(int j=0;j<i;j++){
            // copy it to another string
            char temp[MAX_EVENT_LENGTH];
            strcpy(temp,tokens_pastevents[j].token);

            // tokenise it with all whitespaces to get the command and arguments
            char *args[MAX_ARGS];
            int arg_count = 0;

            args[arg_count] = strtok(temp, " \t\n");
            while (args[arg_count] != NULL && arg_count < MAX_ARGS) {
                arg_count++;
                args[arg_count] = strtok(NULL, " \t\n");
            }

            // if the command is pastevents and the second argument is execute, then replace the command with the command at index <index>
            if(strcmp(args[0],"pastevents")==0 && arg_count>1 && strcmp(args[1],"execute")==0){
                // args[2]="<index>"
                int index=atoi(args[2]);
                if(index>event_count){
                    printf("Index out of bounds\n");
                    continue;
                }
                strcpy(tokens_pastevents[j].token,events[index-1]);
            }

            else{
                // concatenate the tokenised arguemnets to get the minimal token
                char minimal_token[MAX_EVENT_LENGTH];
                minimal_token[0]='\0';
                for(int k=0;k<arg_count;k++){
                    strcat(minimal_token,args[k]);
                    if(k<arg_count-1)
                        strcat(minimal_token," ");
                }

                // copy the minimal token to the tokenised token
                strcpy(tokens_pastevents[j].token,minimal_token);
            }
        }

        // Concatenate all tokens(and their arguments) to get the original command(along with delimiter)
        char original_command[MAX_EVENT_LENGTH];
        original_command[0]='\0';
        
        for(int j=0;j<i;j++){
            char delimiter = tokens_pastevents[j].delimiter;

            strcat(original_command, tokens_pastevents[j].token);
            strcat(original_command, &delimiter);
        }

        // print the original command
        // printf("original command: %s\n",original_command);

       // declare array of structs to store tokens and their delimiters
        struct TokenWithDelimiter tokens[MAX_TOKENS];
        
        i=tokeniser(tokens,original_command);

        int flag=1;

        char delimiter;
        char *args[MAX_ARGS];
        int arg_count = 0;

        // execute all tokens in a loop (autocompleted by Copilot)
        for (int j = 0; j < i; j++) {   
            // get delimiter
            delimiter=tokens[j].delimiter;

            char command[MAX_COMMAND_LENGTH];

            // tokenise the token with whitespaces to get the command and arguments
            strcpy(command, tokens[j].token);
            arg_count = 0;

            args[arg_count] = strtok(command, " \t\n");
            while (args[arg_count] != NULL && arg_count < MAX_ARGS) {
                arg_count++;
                args[arg_count] = strtok(NULL, " \t\n");
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

                else if(strcmp(args[0],"activities")==0){
                    activities(background_processes,background_process_count);
                }

                else if(strcmp(args[0],"pastevents")==0){
                    flag=0;
                    if(arg_count>1 && strcmp(args[1],"purge")==0){
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

                else if(strcmp(args[0],"ping")==0){
                    pid_t pid=atoi(args[1]);
                    int signal_number=atoi(args[2]);

                    ping(pid,signal_number);
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
                
                // if (end_time - start_time > 2) {
                //     printf("Foreground process '%s' took %lds\n", args[0], (long)(end_time - start_time));
                // }
            }
            
        }   

        // if not, add the input to the list of past events
        // also, if the original command contains the work 'pastevents'(apart from ), don't add it to the list
        if (flag) {

            if (event_count == 0 || (event_count>0 && strcmp(events[event_count - 1], original_command) != 0)) {
                // print the last character of the original command
                // printf("last char: %d\n",original_command[strlen(original_command)-1]);
                add_event(original_command, events, &event_count);
                write_past_events(events, event_count, history_file_path);
            }
        }

        print_background_processes();
        // printf("number of background processes: %d\n",background_process_count);
    }

    return 0;
}
