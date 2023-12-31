#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>


// will contain common headers most files require + what main requires
#define MAX_TOKENS 1024
#define MAX_ARGS 64

#define MAX_EVENTS 15
#define FILENAME "pastevents.txt"

#define MAX_EVENT_LENGTH 1000
#define MAX_COMMAND_LENGTH 100

#define MAX_PATH_LENGTH 1024

#define MAX_PROCESSES 100

struct BackgroundProcess {
    pid_t pid;
    char name[MAX_COMMAND_LENGTH];
    char status[10];
};

struct TokenWithDelimiter {
    char token[MAX_COMMAND_LENGTH];
    char delimiter; // Stores either ';' or '&'
};



#include "promptHandler/prompt.h"
#endif