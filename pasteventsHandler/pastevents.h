#ifndef PASTEVENTS_H_
#define PASTEVENTS_H_

# include "../headers.h"
void pastevents();
void add_event( char *, char events[][MAX_EVENT_LENGTH], int *);
void read_past_events(char events[][MAX_EVENT_LENGTH], int *, const char *);
void write_past_events(const char events[][MAX_EVENT_LENGTH], int , const char *);

#endif