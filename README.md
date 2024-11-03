# asgn_1_bsdsf22a031
OS project  Creating UNIX Shell
# My Custom UNIX Shell

## Description
This is a custom implementation of a UNIX shell that supports basic command execution, job control, and variable handling. The shell has been developed across multiple versions, each adding functionality and improving performance.

## Features
### Version 01
The first version of the shell implements the following capabilities:
- **Prompt Display**: The shell program displays a prompt, which is `PUCITshell:-`.
- **Command Execution**: The user can type a command (with options and arguments) in a single line. The shell parses the line into tokens, forks a new process, and passes those tokens to the `exec` family of functions for execution. The parent process waits for the child process to terminate before displaying the prompt again.
- **Exit Shell**: The user can quit the shell by pressing `<CTRL+D>`.

### Version 02
This version enhances the shell by adding input and output redirection:
- **Redirection**: Allows users to redirect `stdin` and `stdout` for new processes using `<` and `>`.
- **Pipes**: Supports the use of pipes for command chaining.

### Version 03
This version introduces background job handling:
- **Background Execution**: Commands can be placed in the background by appending `&` to the command line. 

### Version 04
This version adds command history:
- **Command History**: Users can repeat previously issued commands using `!number`, where the number indicates which command to repeat. The history file maintains the last 10 commands.

### Version 05
This version distinguishes between built-in and external commands:
- **Built-in Commands**: The shell supports built-in commands such as `cd`, `exit`, `jobs`, `kill <pid>`, and `help`.

### Version 06 (Bonus)
This version introduces variable handling:
- **Variable Management**: The shell supports local/user-defined and environment variables. Users can assign values to variables, retrieve them, and list them.
- **Variable Storage**: A system is implemented to distinguish between local and global variables using a data structure.

## Installation Instructions
To compile and run the shell:
```bash
gcc -o myshell myshell.c
./myshell

