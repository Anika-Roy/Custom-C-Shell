#ifndef NEONATE_H_
#define NEONATE_H_

#include "../headers.h"
#include <termios.h>
#include <ctype.h>
#include <dirent.h>

void die(const char *s);
void disableRawMode();
void enableRawMode();
pid_t get_most_recent_pid();
void neonate(int time_interval);

#endif