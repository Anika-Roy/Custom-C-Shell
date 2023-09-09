#include "redir.h"
/*
Specification 9 : I/O Redirection [8]
I/O Redirection is when you change the default input/output (which is the terminal) to another file. This file can be used to read input into a program or to capture the output of a program. This specification should work for all commands - user defined as well as system commands defined in bash. Your shell should support >, <, » (< should work with both > and »).

> : Outputs to the filename following “>”.
>> : Similar to “>” but appends instead of overwriting if the file already exists.
< : Reads input from the filename following “<”.
Your shell should handle these cases appropriately:

An error message “No such input file found!” should be displayed if the input file does not exist.
The output file should be created (with permissions 0644) if it does not already exist in both > and ».
In case the output file already exists, it should be overwritten in case of > and appended to in case of ».
You are NOT required to handle multiple inputs and outputs.
*/
void redir(){

    return;
}