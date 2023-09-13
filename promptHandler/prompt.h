#ifndef __PROMPT_H
#define __PROMPT_H

#include <stdlib.h>
#include <pwd.h>
#include "../headers.h"
#include <ctype.h>

void prompt(char *store_calling_directory);
int tokeniser(struct TokenWithDelimiter[], char[]);
int handle_pastevents_execute_and_tokenise(char* input,char events[][MAX_EVENT_LENGTH],int event_count,struct TokenWithDelimiter tokens[],char original_command[]);

#endif