[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/76mHqLr5)
# Description
Building my own C shell :D

# Assumptions
1. When the ~ flag is used in warp, it'll be used as the first letter only
2. Handling background processes in pastevents: All background commands are stored in pastevents
3. Only single non-chained commands (without ; and &) can be rerun using pastevents execute index
4. < and > can only be used only once in each ; and & separated token, and that too only in the first or last subtoken
5. If a command is not found, it'll be treated as a system command
6. All input output redirections need to be space separated
7. If a prior command in piping or redirection gives an error, redirect or pipe will not be executed completely
8. Redirections and Pipes aren't implemented for user defined functions
9. `exit` is used standalone, so it goes into history as only exit
