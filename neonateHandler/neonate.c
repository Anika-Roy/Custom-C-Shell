#include "neonate.h"

void die(const char *s) {
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

pid_t get_most_recent_pid() {
    // read /proc/loadavg file to get the fifth field(the process id of the most recent process)
    // autcompleted by Copilot

    FILE *fp;

    // open file
    fp = fopen("/proc/loadavg", "r");

    // read the file
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // read the first line
    read = getline(&line, &len, fp);

    // get the fifth field
    char *token = strtok(line, " ");
    for (int i = 0; i < 4; i++) {
        token = strtok(NULL, " ");
    }

    // convert the string to integer
    pid_t pid = atoi(token);

    // close the file
    fclose(fp);

    // printf("%d\n", pid);

    return pid;

    
}


void neonate(int time_interval) {

    // fork a child process
    // the child runs sleep and print, while the parent runs in raw mode

    // Idea to fork, discussed with Radhikesh Agrawal

    // printf("reached here\n");
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // printf("reached child\n");
        // child process
        while(1){
            
            sleep(1);
            // get the pid of the most recent process
            pid_t pid = get_most_recent_pid();
            printf("%d\n", pid);
        }
    }
    else{
        // printf("reached parent\n");
        // parent process
        enableRawMode();

        // wait for user to press 'x'
        char c;

        while (1) {
            c = getchar();
            if (c == 'x') {
                printf("%c", c);
                // kill the child process
                kill(pid, SIGKILL);
                break;
            }
        }

        // restore the terminal settings
        disableRawMode();
    }

    return;

    // check if enable and disable raw mode are working
    // enableRawMode();
    // char c;
    // while (1) {
    //     c = getchar();
        
    //     if (c == 'x') {
    //         printf("%c", c);
    //         break;
    //     }
    // }
    // disableRawMode();

}
