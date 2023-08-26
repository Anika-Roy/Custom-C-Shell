#ifndef HEADERS_H_
#define HEADERS_H_

// will contain common headers most files require + what main requires
#define MAX_TOKENS 1024
#define MAX_ARGS 64

#define MAX_EVENTS 15
#define FILENAME "pastevents.txt"

#define MAX_EVENT_LENGTH 1000
#define MAX_COMMAND_LENGTH 100

struct TokenWithDelimiter {
    char token[MAX_COMMAND_LENGTH];
    char delimiter; // Stores either ';' or '&'
};

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>



#include "promptHandler/prompt.h"
#endif