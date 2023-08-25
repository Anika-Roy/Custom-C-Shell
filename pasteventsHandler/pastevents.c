# include "pastevents.h"
/*
Specification 5 : pastevents commands [8]
pastevents

Implement a ‘pastevents’ command which is similar to the actual history command in bash. Your implementation should store (and output) the 15 most recent command statements given as input to the shell based on some constraints. You must overwrite the oldest commands if more than the set number (15) of commands are entered. pastevents is persistent over different shell runs, i.e., the most recent command statements should be stored when the shell is exited and be retrieved later on when the shell is opened.

Note :

DO NOT store a command in pastevents if it is the exactly same as the previously entered command. (You may use direct string comparison, strcmp(), for this)
Store the arguments along with the command
(Edit) Store all statements except commands that include pastevents or pastevents purge.
pastevents purge

Clears all the pastevents currently stored. Do not store this command in the pastevents.

pastevents execute <index>

Execute the command at position in pastevents (ordered from most recent to oldest). Do not store statements containing `pastevents execute` if it matches the most recent command.
*/
void pastevents(char* args[],int arg_count, char* events[],int event_count,char* history_file_path){

    // args[0]="pastevents"

    // just pastevents
    // if(arg_count==1){
    //     for(int i=0;i<event_count;i++){
    //         printf("%s\n",events[i]);
    //     }
    //     return;
    // }
    // // pastevents purge
    // if(strcmp(args[1],"purge")==0){
    //     event_count=0;
    //     write_past_events(events,event_count,history_file_path);
    //     return;
    // }




    return;
}