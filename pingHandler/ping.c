#include "ping.h"

/*
Specification 13 : Signals [12]
ping <pid> <signal_number>

ping command is used to send signals to processes. Take the pid of a process and send a signal to it which corresponds 
to the signal number (which is provided as an argument). Print error “No such process found”, if process with given pid 
does not exist. You should take signal number’s modulo with 32 before checking which signal it belongs to (assuming x86/ARM machine)
Check man page for signal for an exhaustive list of all signals present.
Example:
```
    <JohnDoe@SYS:~> activities
    221 : emacs new.txt - Running
    430 : vim - Stopped
    620 : gedit - Stopped
    <JohnDoe@SYS:~> ping 221 9             # 9 is for SIGKILL
    Sent signal 9 to process with pid 221
    <JohnDoe@SYS:~> activities
    430 : vim - Stopped
    620 : gedit - Stopped
    <JohnDoe@SYS:~> ping 430 41
    Sent signal 9 to process with pid 430
    <JohnDoe@SYS:~> activities
    620 : gedit - Stopped
```
*/
void ping(pid_t pid, int signal_number){

    // Check if the process with the specified PID exists
    if (kill(pid, 0) == -1) {
        printf("No such process found\n");
        return;
    }

    signal_number = signal_number % 32;

    int kill_status = kill(pid, signal_number);
    if(kill_status == -1){
        printf("Signal couldn't be sent\n");
    }
    else{
        printf("Sent signal %d to process with pid %d\n", signal_number, pid);
    }

}