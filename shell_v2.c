#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "PUCITshell:- "

// Function prototypes
int execute(char* arglist[], char* infile, char* outfile, int pipe_in, int pipe_out);
char** tokenize(char* cmdline);
char* read_cmd(char*, FILE*);

int main() {
    char *cmdline;
    char** arglist;
    char* prompt = PROMPT;
    
    while ((cmdline = read_cmd(prompt, stdin)) != NULL) {
        char *infile = NULL, *outfile = NULL;
        int pipe_in = 0, pipe_out = 0;
        
        // Tokenize command line and handle redirection and pipes
        if ((arglist = tokenize(cmdline)) != NULL) {
            // Check for I/O redirection or piping symbols
            for (int i = 0; arglist[i] != NULL; i++) {
                if (strcmp(arglist[i], "<") == 0) {
                    infile = arglist[i + 1];
                    arglist[i] = NULL;
                } else if (strcmp(arglist[i], ">") == 0) {
                    outfile = arglist[i + 1];
                    arglist[i] = NULL;
                } else if (strcmp(arglist[i], "|") == 0) {
                    pipe_out = 1;
                    arglist[i] = NULL;
                }
            }
            // Execute the command with redirection and piping
            execute(arglist, infile, outfile, pipe_in, pipe_out);

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

int execute(char* arglist[], char* infile, char* outfile, int pipe_in, int pipe_out) {
    int status;
    int pipefd[2];

    // Set up a pipe if needed
    if (pipe_out) {
        if (pipe(pipefd) == -1) {
            perror("Pipe failed");
            exit(1);
        }
    }

    int cpid = fork();
    switch (cpid) {
        case -1:
            perror("Fork failed");
            exit(1);
        
        case 0: // Child process
            if (infile != NULL) { // Input redirection
                int in_fd = open(infile, O_RDONLY);
                if (in_fd == -1) {
                    perror("Failed to open input file");
                    exit(1);
                }
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (outfile != NULL) { // Output redirection
                int out_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (out_fd == -1) {
                    perror("Failed to open output file");
                    exit(1);
                }
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }
            if (pipe_out) { // Pipe out
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            
            execvp(arglist[0], arglist);
            perror("Command not found");
            exit(1);
        
        default: // Parent process
            if (pipe_out) {
                close(pipefd[1]); // Close write end in the parent
                waitpid(cpid, &status, 0);
                int pid2 = fork();
                
                if (pid2 == 0) { // Second child for the piped command
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);
                    execvp(arglist[0], arglist);
                    perror("Command not found in piped command");
                    exit(1);
                }
                close(pipefd[0]);
                waitpid(pid2, &status, 0);
            } else {
                waitpid(cpid, &status, 0);
                printf("Child exited with status %d\n", status >> 8);
            }
            return 0;
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
