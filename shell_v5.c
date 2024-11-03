#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_LINE 1024
#define MAX_ARGS 100
#define MAX_JOBS 10

typedef struct {
    pid_t pid;
    char command[MAX_LINE];
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;

void add_job(pid_t pid, char *command) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strcpy(jobs[job_count].command, command);
        job_count++;
    }
}

void list_jobs() {
    printf("Background jobs:\n");
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %d: %s\n", i + 1, jobs[i].pid, jobs[i].command);
    }
}

void kill_job(pid_t pid) {
    if (kill(pid, SIGTERM) == 0) {
        printf("Terminated job with PID: %d\n", pid);
    } else {
        perror("Failed to kill job");
    }
}

void execute_command(char **args, int arg_count) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        if (arg_count > 1 && strcmp(args[arg_count - 1], "&") == 0) {
            // Remove the '&' from the argument list
            args[arg_count - 1] = NULL; // Null terminate the command without '&'
            printf("[%d] %d: %s\n", job_count + 1, pid, args[0]);
            add_job(pid, args[0]);
        } else {
            // Wait for the child to complete if not a background job
            waitpid(pid, NULL, 0);
        }
    } else {
        perror("fork failed");
    }
}

void change_directory(char *path) {
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

void print_help() {
    printf("Available built-in commands:\n");
    printf("cd <path>  : Change the working directory\n");
    printf("exit       : Terminate the shell\n");
    printf("jobs       : List background jobs\n");
    printf("kill <pid> : Terminate a background job by PID\n");
    printf("help       : Show this help message\n");
}

int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    while (1) {
        // Display the prompt
        printf("PUCITshell@%s:- ", getcwd(NULL, 0));
        fflush(stdout);
        
        // Read input
        if (!fgets(line, MAX_LINE, stdin)) {
            break;  // Exit on CTRL+D
        }

        // Remove trailing newline character
        line[strcspn(line, "\n")] = 0;

        // Tokenize the input
        char *token = strtok(line, " ");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Handle built-in commands
        if (i > 0) {
            if (strcmp(args[0], "cd") == 0) {
                if (i > 1) {
                    change_directory(args[1]);
                } else {
                    printf("cd: missing argument\n");
                }
            } else if (strcmp(args[0], "exit") == 0) {
                break;  // Exit the shell
            } else if (strcmp(args[0], "jobs") == 0) {
                list_jobs();
            } else if (strcmp(args[0], "kill") == 0) {
                if (i > 1) {
                    pid_t pid = atoi(args[1]);
                    kill_job(pid);
                } else {
                    printf("kill: missing argument\n");
                }
            } else if (strcmp(args[0], "help") == 0) {
                print_help();
            } else {
                // Execute command with the argument count
                execute_command(args, i);
            }
        }
    }
    return 0;
}
