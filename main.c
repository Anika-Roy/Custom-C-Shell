#include "headers.h"
#include "fg_bg_Handler/utils.h"
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

int backup_input;
int backup_output;

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
        insert_background_process(child_pid, command,&background_process_count,background_processes);

        printf("[%d] %d\n", background_process_count, child_pid);
        
    }
}

void execute_foreground(char* args[], pid_t shell_pid){
    // only execvp

    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);

    
    } else if (child_pid == 0) {
        // Child process
        
        // set process group id to its own pid
        setpgid(child_pid,child_pid);

        pid_t my_pid = getpid();

        signal(SIGTTOU, SIG_IGN);
        // Bring the group to the foreground
        tcsetpgrp(backup_input, my_pid);

        // fprintf(stderr, "PID %s",strerror(errno));

        // write(1,"hello\n",6);

        // pid_t my_pid = getpid();

        // print the args
        // for(int i=0;args[i]!=NULL;i++){
        //     printf("%d->%s\n",i,args[i]);
        // }

        // printf("called execvp!\n");

        int error_flag = execvp(args[0], args);
        // if error occurs, print error
        if (error_flag == -1) {
            printf("ERROR : '%s' is not a valid command\n",args[0]);
        }
        
        exit(EXIT_FAILURE);
        
    }
    else {

        // printf("reached parent\n");
        wait(NULL);

        signal(SIGTTOU, SIG_IGN);
        // Give control back to the shell
        tcsetpgrp(backup_input, shell_pid);

        return;
    }
}

void handle_signal(int signum) {
    switch (signum) {
        case SIGCHLD:
            // printf("SIGCHLD received\n");
            // Handle child process terminated (SIGCHLD)
            // Add your handling code here
            check_background_processes_sync(&background_process_count,background_processes);
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

    // store the pid of the shell
    pid_t shell_pid = getpid(); 
    backup_input = STDIN_FILENO; 
    backup_output = STDOUT_FILENO;

    while (1)
    {
        // printf("starting again!\n");
        // Print appropriate prompt with username, systemname and directory before accepting input
        // printf("background_process_count: %d\n",background_process_count );
        prompt(store_calling_directory);
        char input[4096];
        fgets(input, 4096, stdin);

        //continue if input is empty
        if (strcmp(input, "\n") == 0)
            continue;

        char original_command[MAX_EVENT_LENGTH];
        original_command[0]='\0';

        // declare array of structs to store tokens and their delimiters
        struct TokenWithDelimiter tokens[MAX_TOKENS];

        int i = handle_pastevents_execute_and_tokenise(input,events,event_count,tokens,original_command);

        int flag=1;

        char delimiter;
        // printf("i: %d\n",i);
        
        // execute all tokens in a loop (autocompleted by Copilot)
        for (int j = 0; j < i; j++) {   
            // get delimiter
            delimiter=tokens[j].delimiter;

            // declare an array of pipe separated commands
            struct PipeSeparatedCommands pipe_separated_commands[MAX_NUM_PIPES];

            // populate it by calling a function
            int num_pipes=pipe_separated_commands_populator(tokens[j].token,pipe_separated_commands);

            // printf("%d\n",num_pipes);
            // print all pipe separated commands and their arguments for debugging
            // printf("num pipes: %d\n",num_pipes);
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
                    // delimiter = tokens[j].delimiter;

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
            // single pipe
            else{
                int k=0;
                

                // If the delimiter is '&', execute the command in the background
                if (delimiter == '&') {
                    // printf("background process\n");
                    execute_background(pipe_separated_commands[k].args);
                    continue;
                }
                // printf("%s\n",pipe_separated_commands[k].args[0]);
                
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

                // If the command is peek, call the peek function
                else if (strcmp(pipe_separated_commands[k].args[0], "peek") == 0) {
                    peek(pipe_separated_commands[k].args, pipe_separated_commands[k].numArgs, store_previous_directory,store_calling_directory);
                    continue;
                }

                else if(strcmp(pipe_separated_commands[k].args[0],"exit")==0){
                    exit(0);
                }

                // If the command is seek, call the seek function
                else if (strcmp(pipe_separated_commands[k].args[0], "seek") == 0) {
                    seek(pipe_separated_commands[k].args, pipe_separated_commands[k].numArgs,store_calling_directory);
                    continue;
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
                    continue;
                }
                else if(strcmp(pipe_separated_commands[k].args[0],"activities")==0){
                    activities(background_processes,background_process_count);
                    continue;
                }
                else{
                    
                    // Checking for redirection block (from ChatGPT)
                    // Check for input and output redirection symbols within args
                    char* input_file = NULL;
                    char* output_file = NULL;

                    int input_fd;
                    int output_fd;

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
                        } 
                        else if (strcmp(pipe_separated_commands[k].args[i], ">") == 0) {
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
                        input_fd = open(input_file, O_RDONLY);
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
                        output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (output_fd == -1) {
                            perror("open");
                            exit(EXIT_FAILURE);
                        }
                        dup2(output_fd, STDOUT_FILENO);
                        close(output_fd);
                    }
                    
                    execute_foreground(pipe_separated_commands[k].args,shell_pid);

                    dup2(saved_stdin, STDIN_FILENO);
                    dup2(saved_stdout, STDOUT_FILENO);
                    close(saved_stdin);
                    close(saved_stdout);
                    continue;
                }

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

        print_background_processes(&background_process_count,background_processes);
        // printf("number of background processes: %d\n",background_process_count);
    }

    return 0;
}

