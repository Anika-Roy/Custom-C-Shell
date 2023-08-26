#ifndef __PROMPT_H
#define __PROMPT_H

#include <stdlib.h>
#include <pwd.h>
#include "../headers.h"
#include <ctype.h>

void prompt(char *store_calling_directory);
int tokeniser(struct TokenWithDelimiter[], char[]);

#endif