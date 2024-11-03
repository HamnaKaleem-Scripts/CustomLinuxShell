#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 100
#define MAX_VARS 100

typedef struct {
    char *name;
    char *value;
    int global; // 1 for global variable, 0 for local variable
} Variable;

Variable variables[MAX_VARS];
int var_count = 0;

void set_variable(char *name, char *value, int global) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            free(variables[i].value);
            variables[i].value = strdup(value);
            variables[i].global = global;
            return;
        }
    }
    // Add new variable
    variables[var_count].name = strdup(name);
    variables[var_count].value = strdup(value);
    variables[var_count].global = global;
    var_count++;
}

char* get_variable(char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].value;
        }
    }
    return NULL; // Not found
}

void list_variables() {
    printf("User-defined variables:\n");
    for (int i = 0; i < var_count; i++) {
        printf("%s=%s\n", variables[i].name, variables[i].value);
    }
}

void execute_command(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        if (args[1] != NULL && strcmp(args[1], "&") == 0) {
            args[1] = NULL; // Remove '&'
            waitpid(pid, NULL, 0); // Wait for the child process to finish
        } else {
            waitpid(pid, NULL, 0);
        }
    } else {
        perror("fork failed");
    }
}

int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];

    while (1) {
        printf("PUCITshell@%s:- ", getcwd(NULL, 0));
        fflush(stdout);
        
        if (!fgets(line, MAX_LINE, stdin)) {
            break; // Exit on CTRL+D
        }

        line[strcspn(line, "\n")] = 0; // Remove trailing newline
        
        char *token = strtok(line, " ");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Handle built-in commands
        if (i > 0) {
            if (strcmp(args[0], "set") == 0 && i > 2) {
                set_variable(args[1], args[2], 0); // Default to local variable
            } else if (strcmp(args[0], "setglobal") == 0 && i > 2) {
                set_variable(args[1], args[2], 1); // Set as global variable
            } else if (strcmp(args[0], "get") == 0 && i > 1) {
                char *value = get_variable(args[1]);
                if (value) {
                    printf("%s=%s\n", args[1], value);
                } else {
                    printf("Variable %s not found.\n", args[1]);
                }
            } else if (strcmp(args[0], "list") == 0) {
                list_variables();
            } else if (strcmp(args[0], "exit") == 0) {
                break; // Exit the shell
            } else {
                execute_command(args);
            }
        }
    }

    // Free allocated memory
    for (int i = 0; i < var_count; i++) {
        free(variables[i].name);
        free(variables[i].value);
    }
    return 0;
}
