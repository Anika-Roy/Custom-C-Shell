# include "../pastevents.h"

void add_event( char *event, char events[][MAX_EVENT_LENGTH], int *count) {   
    if (*count >= MAX_EVENTS) {
        // Remove the oldest event
        for (int i = 1; i < MAX_EVENTS; i++) {
            strcpy(events[i - 1], events[i]);
        }
        (*count)--;
    }
    strcpy(events[*count], event);
    (*count)++;
}

void read_past_events(char events[][MAX_EVENT_LENGTH], int *count, const char *history_file_path) {
    FILE *file = fopen(history_file_path, "r");
    if (file != NULL) {
        *count = 0;
        while (*count < MAX_EVENTS && fgets(events[*count], sizeof(events[*count]), file)) {
            (*count)++;
        }
        //remove newline from the end
        for(int i=0;i<*count;i++){
            events[i][strlen(events[i])-1]='\0';
        }
        fclose(file);
    }
}

void write_past_events(const char events[][MAX_EVENT_LENGTH], int count, const char *history_file_path) {
    FILE *file = fopen(history_file_path, "w");
    if (file != NULL) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s\n", events[i]);
        }
        fclose(file);
    }
}
