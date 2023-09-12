#include "headers.h"
#include "promptHandler/prompt.h"
#include "warpHandler/warp.h"
#include "peekHandler/peek.h"
#include "pasteventsHandler/pastevents.h"
#include "seekHandler/seek.h"
#include "activitiesHandler/activities.h"
#include "pingHandler/ping.h"
#include "pipeHandler/pipeHandler.h"
#include "iManHandler/iMan.h"
#include "neonateHandler/neonate.h"

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


void print_background_processes(){
    for (int i = 0; i < background_process_count; i++) {
        if(strcmp(background_processes[i].status,"Running")!=0 && strcmp(background_processes[i].status,"Stopped")!=0){
            if(strcmp(background_processes[i].status,"Finished")==0)
                printf("%s (PID %d) exited normally\n", background_processes[i].name, background_processes[i].pid);
            else if(strcmp(background_processes[i].status,"Failed")==0)
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
            // printf("SIGCHLD received\n");
            // Handle child process terminated (SIGCHLD)
            // Add your handling code here
            check_background_processes_sync();
            break;
        case SIGTSTP: // Ctrl+Z
            printf("SIGTSTP received\n");
            
            break;
        case SIGINT: // Ctrl+C
            /* Interrupt any currently running foreground process by sending it the SIGINT signal. 
            It has no effect if no foreground process is currently running.*/
            printf("SIGINT received\n");

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
    sigaction(SIGTSTP, &sa, NULL); // Handle Ctrl+Z (SIGTSTP)
    // sigaction(SIGINT, &sa, NULL); // Handle Ctrl+C (SIGINT)


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
        // if (user_input == Ctrl+D) {
        // // Handle Ctrl+D
        // // Send termination signals to background processes
        // for each background_process in background_processes {
        //     send_termination_signal(background_process);
        // }

        // // Wait for background processes to exit
        // wait_for_background_processes_to_exit();

        // // Exit your shell program
        // exit(EXIT_SUCCESS);
        // }

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

        // print tokens in tokens_pastevents
        // printf("tokens in tokens_pastevents:\n");
        // for(int j=0;j<i;j++){
        //     printf("token: %s\n",tokens_pastevents[j].token);
        //     printf("delimiter: %c\n",tokens_pastevents[j].delimiter);
        // }

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
                strcpy(tokens_pastevents[j].token,events[index-1]); //[TODO: Handle delimiter clashes]
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
                // printf("minimal token: %s\n",minimal_token);

                // copy the minimal token to the tokenised token
                strcpy(tokens_pastevents[j].token,minimal_token);
                // printf("token: %s\n",tokens_pastevents[j].token);
                // printf("delimiter: %c\n",tokens_pastevents[j].delimiter);
            }
        }

        // Concatenate all tokens(and their arguments) to get the original command(along with delimiter)
        char original_command[MAX_EVENT_LENGTH];
        original_command[0]='\0';
        
        for(int j=0;j<i;j++){
            char delimiter = tokens_pastevents[j].delimiter;

            strcat(original_command, tokens_pastevents[j].token);
            // if(j<i-1)
            //     strcat(original_command, &delimiter); [TODO: I had added this to debug, but forgot why, not its causing problems with sending things to background]
            strcat(original_command, &delimiter);
        }

        // print the original command
        // printf("original command: %s\n",original_command);

       // declare array of structs to store tokens and their delimiters
        struct TokenWithDelimiter tokens[MAX_TOKENS];


        // printf("%d\n",i);
        i=tokeniser(tokens,original_command);
        // printf("%d\n",i);

        int flag=1;

        char delimiter;
        
        // execute all tokens in a loop (autocompleted by Copilot)
        for (int j = 0; j < i; j++) {   
            // get delimiter
            delimiter=tokens[j].delimiter;

            // declare an array of pipe separated commands
            struct PipeSeparatedCommands pipe_separated_commands[MAX_NUM_PIPES];

            // populate it by calling a function
            int num_pipes=pipe_separated_commands_populator(tokens[j].token,pipe_separated_commands);

            // // print all pipe separated commands and their arguments for debugging
            // for(int k=0;k<num_pipes;k++){
            //     // printf("command: %s\n",pipe_separated_commands[k].command);
            //     for(int l=0;l<pipe_separated_commands[k].numArgs;l++){
            //         printf("args %d -> %s\n",l,pipe_separated_commands[k].args[l]);
            //     }
            // }


            // execute all pipe separated commands in a loop -- using ChatGPT

            // if num_pipes>1, only then create pipes
            if (num_pipes>1){
                
                int input_fd = STDIN_FILENO;
                // int saved_stdin = dup(STDIN_FILENO); // Save the original stdin
                int output_fd = STDOUT_FILENO;
                // int saved_stdout = dup(STDOUT_FILENO); // Save the original stdout

                // int error_flag=0; // in case piping or redirection cause any errors, the pipe will not be completely executed
                for(int k=0 ; k<num_pipes ; k++){
                    
                    // check for file redirection if it is the first pipe
                    if(k==0){
                        char* input_file = NULL;
                        // [TODO: Error handling]
                        for (int i = 0; i < pipe_separated_commands[k].numArgs; i++) {
                            if (strcmp(pipe_separated_commands[k].args[i], "<") == 0) {
                                // Found input redirection symbol '<'
                                if (i + 1 < pipe_separated_commands[k].numArgs) {
                                    input_file = pipe_separated_commands[k].args[i + 1];
                                    pipe_separated_commands[k].args[i] = NULL; // Null-terminate the command
                                    // break;
                                } else {
                                    printf("Error: Missing input file after '<'\n");
                                    exit(EXIT_FAILURE);
                                }
                            } 
                        }

                        if (input_file) {
                            // Open the input file and associate it with STDIN_FILENO
                            input_fd = open(input_file, O_RDONLY);
                            if (input_fd == -1) {
                                perror("open");
                                exit(EXIT_FAILURE);
                            }
                            // dup2(input_fd, STDIN_FILENO);
                            // close(input_fd);
                        }
                    }

                    // check for file redirection if it is the last pipe
                    if(k==num_pipes-1){
                        char* output_file = NULL;
                        // [TODO: Error handling]
                        for (int i = 0; i < pipe_separated_commands[k].numArgs; i++) {
                            if (strcmp(pipe_separated_commands[k].args[i], ">") == 0) {
                                // Found output redirection symbol '>'
                                if (i + 1 < pipe_separated_commands[k].numArgs) {
                                    output_file = pipe_separated_commands[k].args[i + 1];
                                    pipe_separated_commands[k].args[i] = NULL; // Null-terminate the command
                                    break;
                                } else {
                                    printf("Error: Missing output file after '>'\n");
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }

                        if (output_file) {
                            // printf("output file: %s\n",output_file);
                            // Open the output file and associate it with STDOUT_FILENO
                            output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (output_fd == -1) {
                                perror("open");
                                exit(EXIT_FAILURE);
                            }
                            // dup2(output_fd, STDOUT_FILENO);
                            // close(output_fd);
                        }
                    }

                    // printf("input_fd: %d\n",input_fd);
                    // printf("output_fd: %d\n",output_fd);

                    // in case we need to put it in background
                    delimiter = tokens[j].delimiter;

                    if (k < num_pipes - 1) {
                        // Create a pipe for communication between commands
                        if (pipe(pipe_separated_commands[k].pipe_fds) == -1) {
                            perror("pipe");
                            exit(EXIT_FAILURE);
                            // error_flag=1;
                        }
                    }

                    pid_t child_pid = fork();
                    if (child_pid < 0) {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    } else if (child_pid == 0) {
                        // Child process

                        // Handle input redirection if needed
                        if (k > 0) {
                            dup2(pipe_separated_commands[k - 1].pipe_fds[0], STDIN_FILENO);
                        } else {
                            dup2(input_fd, STDIN_FILENO); // Redirect input from file or stdin
                        }

                        // Handle output redirection if needed
                        if (k < num_pipes - 1) {
                            dup2(pipe_separated_commands[k].pipe_fds[1], STDOUT_FILENO);
                        } else {
                            dup2(output_fd, STDOUT_FILENO); // Redirect output to file or stdout
                        }

                        // Execute the command
                        int error_flag = execvp(pipe_separated_commands[k].args[0], pipe_separated_commands[k].args);
                        // if error occurs, print error
                        if (error_flag == -1) {
                            printf("ERROR : '%s' is not a valid command\n",pipe_separated_commands[k].args[0]);
                            exit(EXIT_FAILURE);
                        }
                        exit(EXIT_SUCCESS);
                    }

                    else{
                        // Parent Process
                        // If it's not the last command, close the write end of the current pipe
                        if (k < num_pipes - 1) {
                            close(pipe_separated_commands[k].pipe_fds[1]);
                        }
                        // If it's not the first command, close the read end of the previous pipe
                        if (k > 0) {
                            close(pipe_separated_commands[k - 1].pipe_fds[0]);
                        }
                        // Wait for the child to complete
                        wait(NULL); // [TODO] Handle errors in piping
                    }

                }
            }

            else{
                int k=0;

                // Checking for redirection block (from ChatGPT)
                // Check for input and output redirection symbols within args
                char* input_file = NULL;
                char* output_file = NULL;

                int saved_stdin = dup(STDIN_FILENO); // Save the original stdin
                int saved_stdout = dup(STDOUT_FILENO); // Save the original stdout

                // [TODO: Error handling][TODO: add append more too]
                for (int i = 0; i < pipe_separated_commands[k].numArgs; i++) {
                    if (strcmp(pipe_separated_commands[k].args[i], "<") == 0) {
                        // Found input redirection symbol '<'
                        if (i + 1 < pipe_separated_commands[k].numArgs) {
                            input_file = pipe_separated_commands[k].args[i + 1];
                            pipe_separated_commands[k].args[i] = NULL; // Null-terminate the command
                            // break;
                        } else {
                            printf("Error: Missing input file after '<'\n");
                            exit(EXIT_FAILURE);
                        }
                    } else if (strcmp(pipe_separated_commands[k].args[i], ">") == 0) {
                        // Found output redirection symbol '>'
                        if (i + 1 < pipe_separated_commands[k].numArgs) {
                            output_file = pipe_separated_commands[k].args[i + 1];
                            pipe_separated_commands[k].args[i] = NULL; // Null-terminate the command
                            break;
                        } else {
                            printf("Error: Missing output file after '>'\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }

                // printf("input file: %s\n",input_file);
                // printf("output file: %s\n",output_file);

                // Now you can handle input and output redirection as needed
                if (input_file) {
                    // Open the input file and associate it with STDIN_FILENO
                    int input_fd = open(input_file, O_RDONLY);
                    if (input_fd == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                }

                if (output_file) {
                    // printf("output file: %s\n",output_file);
                    // Open the output file and associate it with STDOUT_FILENO
                    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (output_fd == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);
                }
                // If the delimiter is '&', execute the command in the background
                if (delimiter == '&') {
                    // printf("background process\n");
                    execute_background(pipe_separated_commands[k].args);
                    continue;
                }
                
                if(strcmp(pipe_separated_commands[k].args[0],"ping")==0){
                    pid_t pid=atoi(pipe_separated_commands[k].args[1]);
                    int signal_number=atoi(pipe_separated_commands[k].args[2]);

                    ping(pid,signal_number);
                    continue;
                }

                // If the command is warp, call the warp function
                else if (strcmp(pipe_separated_commands[k].args[0], "warp") == 0) {
                    warp(pipe_separated_commands[k].args, pipe_separated_commands[k].numArgs, store_calling_directory, store_previous_directory);
                    continue;
                }

                else if(strcmp(pipe_separated_commands[k].args[0],"iMan")==0){
                    fetch_man_page(pipe_separated_commands[k].args[1]);
                    continue;
                }

                else if(strcmp(pipe_separated_commands[k].args[0],"neonate")==0){
                    // print the 1 and 2nd indexed arguements
                    // printf("%s %s\n",pipe_separated_commands[k].args[1],pipe_separated_commands[k].args[2]);

                    int time_interval= atoi(pipe_separated_commands[k].args[2]);
                    neonate(time_interval);
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
                    if(strcmp(pipe_separated_commands[k].args[0],"exit")==0){
                        exit(0);
                    }

                    // If the command is peek, call the peek function
                    else if (strcmp(pipe_separated_commands[k].args[0], "peek") == 0) {
                        peek(pipe_separated_commands[k].args, pipe_separated_commands[k].numArgs, store_previous_directory,store_calling_directory);
                    }

                    // If the command is seek, call the seek function
                    else if (strcmp(pipe_separated_commands[k].args[0], "seek") == 0) {
                        seek(pipe_separated_commands[k].args, pipe_separated_commands[k].numArgs,store_calling_directory);
                    }

                    else if(strcmp(pipe_separated_commands[k].args[0],"activities")==0){
                        activities(background_processes,background_process_count);
                    }

                    else if(strcmp(pipe_separated_commands[k].args[0],"pastevents")==0){
                        flag=0;
                        if(pipe_separated_commands[k].numArgs>1 && strcmp(pipe_separated_commands[k].args[1],"purge")==0){
                            event_count=0;
                            write_past_events(events,event_count,history_file_path);
                            continue;
                        }
                        else {
                            if(pipe_separated_commands[k].numArgs==1){
                                for(int i=0;i<event_count;i++){
                                    printf("%s\n",events[i]);   
                                }
                            }
                        }
                    }

                    // else if(strcmp(args[0],"ping")==0){
                    //     pid_t pid=atoi(args[1]);
                    //     int signal_number=atoi(args[2]);

                    //     ping(pid,signal_number);
                    // }

                    else{
                        // execute using execvp
                        int error_flag = execvp(pipe_separated_commands[k].args[0], pipe_separated_commands[k].args);
                        // if error occurs, print error
                        if (error_flag == -1) {
                            printf("ERROR : '%s' is not a valid command\n",pipe_separated_commands[k].args[0]);
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

                // Reset stdin and stdout
                dup2(saved_stdin, STDIN_FILENO);
                dup2(saved_stdout, STDOUT_FILENO);
                close(saved_stdin);
                close(saved_stdout);
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
    }

    return 0;
}

