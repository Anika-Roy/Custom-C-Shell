#ifndef __PROMPT_H
#define __PROMPT_H

#include <stdlib.h>
#include <pwd.h>
#include "../headers.h"
#include <ctype.h>

void prompt();
int tokeniser(struct TokenWithDelimiter[], char[]);

#endif