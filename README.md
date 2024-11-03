# asgn_1_bsdsf22a031
OS project  Creating UNIX Shell
# My Custom UNIX Shell

## Description
This is a custom implementation of a UNIX shell that supports basic command execution, job control, and variable handling. The shell has been developed across multiple versions, each adding functionality and improving performance.

## Features

### Version 01
- Basic command execution: Run commands like `ls`, `pwd`, etc.
- Simple prompt to take user input.

### Version 02
- Support for background jobs using `&`.
- Handling of built-in commands:
  - `cd`: Change directory
  - `exit`: Terminate the shell

### Version 03
- Implementation of the `jobs` command to list background jobs.
- Enhanced error handling for commands and job control.

### Version 04
- Addition of the `kill <pid>` command to terminate background jobs.
- Improved feedback for job status and termination.

### Version 05
- Implementation of environment variables for more flexible shell behavior.
- User-defined variable support, allowing users to create and manage their own variables.

### Version 06 (Bonus)
- Introduction of a variable storage system, allowing for local/user-defined and global variables.
- Ability to access and modify both local and environment variables within the shell.

## Installation Instructions
To compile and run the shell, use the following commands:
```bash
gcc -o myshell myshell.c
./myshell
