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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        i++;
        if (i == 1) {
            // printf("%s", line);
            break;
        }
    }

    fclose(fp);
    if (line)
        free(line);

    char *token = strtok(line, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");

    pid_t pid = atoi(token);

    return pid;
}


void neonate(int time_interval) {

    // fork a child process
    // the child runs sleep and print, while the parent runs in raw mode

    // Idea to fork, discussed with Radhikesh Agrawal

    printf("reached here\n");
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        printf("reached child\n");
        // child process
        while(1){
            sleep(time_interval);
            // get the pid of the most recent process
            pid_t pid = get_most_recent_pid();
            
            printf("%d\n", pid);
        }
    }
    else{
        printf("reached parent\n");
        // parent process
        enableRawMode();

        // wait for user to press 'x'
        char c;

        while (1) {
            c = getchar();
            printf("%c", c);
            if (c == 'x') {
                // kill the child process
                kill(pid, SIGKILL);
                break;
            }
        }

        // restore the terminal settings
        disableRawMode();
    }

    return;
}