#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "PUCITshell:- "

// Function prototypes
int execute(char* arglist[], int background);
char** tokenize(char* cmdline);
char* read_cmd(char*, FILE*);

// Signal handler to clean up background processes
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    char *cmdline;
    char** arglist;
    char* prompt = PROMPT;

    // Register signal handler for SIGCHLD
    signal(SIGCHLD, sigchld_handler);
    
    while ((cmdline = read_cmd(prompt, stdin)) != NULL) {
        int background = 0;

        // Tokenize command line
        if ((arglist = tokenize(cmdline)) != NULL) {
            // Check if the last argument is "&" for background execution
            int last_arg = 0;
            while (arglist[last_arg] != NULL) last_arg++;
            if (last_arg > 0 && strcmp(arglist[last_arg - 1], "&") == 0) {
                background = 1;
                arglist[last_arg - 1] = NULL; // Remove "&" from argument list
            }

            // Execute the command with background option
            execute(arglist, background);

            // Free dynamically allocated memory for tokens
            for (int j = 0; j < MAXARGS + 1; j++)
                free(arglist[j]);
            free(arglist);
            free(cmdline);
        }
    }
    printf("\n");
    return 0;
}

int execute(char* arglist[], int background) {
    int status;
    int cpid = fork();

    switch (cpid) {
        case -1:
            perror("Fork failed");
            exit(1);
        
        case 0: // Child process
            execvp(arglist[0], arglist);
            perror("Command not found");
            exit(1);
        
        default: // Parent process
            if (background) {
                printf("[1] %d\n", cpid); // Display background process ID
                return 0; // Don't wait for background processes
            } else {
                waitpid(cpid, &status, 0);
                printf("Child exited with status %d\n", status >> 8);
                return 0;
            }
    }
}

char** tokenize(char* cmdline) {
    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int j = 0; j < MAXARGS + 1; j++) {
        arglist[j] = (char*)malloc(sizeof(char) * ARGLEN);
        bzero(arglist[j], ARGLEN);
    }
    
    if (cmdline[0] == '\0') // If user has entered nothing and pressed enter
        return NULL;

    int argnum = 0; // Slots used
    char* cp = cmdline; // Position in string
    char* start;
    int len;

    while (*cp != '\0') {
        while (*cp == ' ' || *cp == '\t') // Skip leading spaces
            cp++;
        start = cp; // Start of the word
        len = 1;
        
        // Find the end of the word
        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t'))
            len++;
        
        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }
    arglist[argnum] = NULL;
    return arglist;
}

char* read_cmd(char* prompt, FILE* fp) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd)); // Get current working directory
    printf("%s%s", cwd, prompt);

    int c; // Input character
    int pos = 0; // Position of character in cmdline
    char* cmdline = (char*)malloc(sizeof(char) * MAX_LEN);

    while ((c = getc(fp)) != EOF) {
        if (c == '\n')
            break;
        cmdline[pos++] = c;
    }
    if (c == EOF && pos == 0)
        return NULL;

    cmdline[pos] = '\0';
    return cmdline;
}
