/* ICCS227: Project 1: icsh
 * Name: Pearploy Chaicharoensin
 * StudentID: 6381278
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>


#define MAX_CMD_BUFFER 255
#define MAX_LINE_LENGTH 1000

pid_t foregroundJob = 0; //global variables keep track of foregroundJob & process ID

char** toTokens(char* buffer) {
    char** toReturn = malloc(MAX_CMD_BUFFER * sizeof(char*));
    char* token = strtok(buffer, " \t\n");
    int i = 0;
    while (token != NULL) {
        toReturn[i] = token;
        token = strtok(NULL, " \t\n");
        i++;
    }
    toReturn[i] = NULL;
    return toReturn;
}

char** copyTokens(char** tokens) {
    char** toReturn = malloc(MAX_CMD_BUFFER * sizeof(char*));
    int i = 0;
    while (tokens[i] != NULL) {
        int len = strlen(tokens[i]);
        toReturn[i] = malloc((len + 1) * sizeof(char));
        strcpy(toReturn[i], tokens[i]);
        i++;
    }
    toReturn[i] = NULL;
    return toReturn;
}

void printToken(char** token, int start) {
    for (int i = start; token[i] != NULL; i++) {
        printf("%s ", token[i]);
    }
    printf("\n");
}

/* Handle command that already exist */
void externalRunning(char** args){ //commandArr
    int status;
    pid_t pid;

    /* Create a process space for the ls */
    if ((pid=fork()) < 0)
    {
        perror ("Fork failed");
        exit(1);
    }
    if (!pid)
    {
    /* This is the child, so execute the ls */
        status = execvp (args[0], args);
        if (status < 0) {
            printf("bad command\n");
        }
        exit(1);
    }

    if (pid)
    {
        /* 
         * We're in the parent; let's wait for the child to finish
         */
        foregroundJob = pid; //update 
        waitpid(pid, &status, 0);
        foregroundJob = 0;
    }
}

void command(char** current, char** prev) {
    /* Turns prev to a string */
    char* prev_output = calloc(MAX_LINE_LENGTH, sizeof(char));
    strcpy(prev_output, "");
    for (int i = 1; prev[i] != NULL; i++) {
        strcat(prev_output, prev[i]);
        strcat(prev_output, " ");
    }

    /* !! */
    if (strcmp(current[0], "!!") == 0 && current[1] == NULL) {
        if (strcmp(prev_output, "") != 0) {
            printf("%s\n", prev_output);
        } else {
            printf("No previous output\n");
        }
    } else {
        
        if (strcmp(current[0], "echo") == 0 && current[1] != NULL) {
            /* echo $? */
            if (strcmp(current[1], "$?") == 0 && current[2] == NULL) {
                 printf("%d\n", 0);
            } 
            else if (strcmp(current[1], "$?") == 0 && current[2] != NULL) {
                printToken(current, 1);
            }
            /* echo */
            else if (strcmp(current[1], "$?") != 0){
                printToken(current, 1);
            }           
        /* exit */
        } else if (strcmp(current[0], "exit") == 0) {
            if (current[1] == NULL) {
                printf("$ echo $?\n0\n$\n");
                exit(0);
            }
            else if (current[2] == NULL)
            {
                printf("$ echo $?\n%s\n$\n",current[1]);
                int num = atoi(current[0]);
                if (num > 255) {
                    num = num & 0xFF;
                }
                exit(num);
            }
        } else {
            externalRunning(current);
        }
    }

    free(prev_output);
}

//similar to main in tag 0.1.0
void readScripts(char* fileName){
    FILE *file;
	file = fopen(fileName, "r");
    if (file == NULL) { printf("Invalid Filename\n");}
    else {
        char buffer[MAX_CMD_BUFFER];
        char** prevBufferArr = toTokens(buffer); 
        while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
            char** curBufferArr = toTokens(buffer); 
            command(curBufferArr, prevBufferArr);
            prevBufferArr = copyTokens(curBufferArr);
            free(curBufferArr);
        }
        fclose(file);
    }
    return;
}


/* SIGNAL */
void SIGTSTP_handler(int signum){
    if (foregroundJob) {
        kill(foregroundJob,SIGTSTP);
        printf("\n");
    }
}
void SIGINT_handler(int signum){
    if (foregroundJob) {
        kill(foregroundJob,SIGINT);
        printf("\n");
    }
}

void signalHandlerSetUP(){
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    /* ctrl+C */
    sa.sa_handler = SIGINT_handler;
    sigaction(SIGINT, &sa, NULL);
    /* ctrl+Z */
    sa.sa_handler = SIGTSTP_handler;
    sigaction(SIGTSTP, &sa, NULL);
}


int main(int arg, char *argv[]) {
    signalHandlerSetUP();
    /* Script */
    if (arg > 1) { 
        readScripts(argv[1]);
    }
    /* User Input */
    else {
        char buffer[MAX_CMD_BUFFER];
        char** prevBufferArr = toTokens(buffer); 
        printf("Starting IC shell\n");
        while (1) {
            printf("icsh $ ");
            fgets(buffer, MAX_CMD_BUFFER, stdin);
            char** curBufferArr = toTokens(buffer); 
            command(curBufferArr, prevBufferArr);
            prevBufferArr = copyTokens(curBufferArr);
            free(curBufferArr);        
        }
    }
    return 0;
}

