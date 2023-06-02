/* ICCS227: Project 1: icsh
 * Name: Pearploy Chaicharoensin
 * StudentID: 6381278
 * Tag : 0.6.0
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_CMD_BUFFER 255
#define MAX_LINE_LENGTH 255 

/* Global variables */
char** prevBufferArr;
char** curBufferArr;
int isRedir = 0;
int foregroundJob = 0;
int jobID = 0; //count jobID

/* Functions must be declared before being called inside other functions */
void command(char** , char** );
void readScripts(char*);
void signalhandler(int);

typedef struct BG
{
    int jobID;
    char* processStatus;
    char* commands;
    int pid;
    int isStopped;
} job;

job jobList[100]; //limit 100 jobs only

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

char** getLines(char* buffer) {
    char** toReturn = malloc(MAX_CMD_BUFFER * sizeof(char*));
    char* token = strtok(buffer, "\n");
    int i = 0;
    while (token != NULL) {
        toReturn[i] = token;
        token = strtok(NULL, "\n");
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

char* tokenStr(char** token, int from) {
    char* toReturn = calloc(MAX_LINE_LENGTH, sizeof(char));
    strcpy(toReturn, "");
    for (int i = from; token[i] != NULL; i++) {
        strcat(toReturn, token[i]);
        strcat(toReturn, " ");
    }
    return toReturn;
}

char* removeFirst(char* input) {
    char* toReturn = calloc(MAX_LINE_LENGTH, sizeof(char));
    //toReturn[0] = '\0'; 
    int len = strlen(input);
    for (int i = 0; i <= len; i++) {
        toReturn[i] = input[i+1];
    }
    return toReturn;
}

/* I/O Redirection */
void redir(char** args){
    int in;
    int out;
    char* fileName;
    char buffer[1024];
    int i = 0 ;
    /*go through each charactor*/
    while ( args[i] != NULL ){
        /*INPUT*/
        if (strcmp(args[i],"<") == 0 && args[i+1] != NULL) {
            in = open(args[i+1],O_RDONLY); 
            if (in <= 0) {
                fprintf (stderr, "Couldn't open a file\n");
                exit (1);
            } 
            fileName = args[i+1];
            dup2(in,0);
            close(in);
        /*OUTPUT*/
        }else if (strcmp(args[i],">") == 0 && args[i+1] != NULL) {
            out = open(args[i+1],O_TRUNC | O_CREAT | O_WRONLY, 0666); 
            if (out <= 0) {
                fprintf (stderr, "Couldn't open a file\n");
                exit(1);
            } 
            dup2(out,1);
            close(out);
        }
        i++;
    } 

    /* Read from inputFile write on Buffer and print out to outputFile */
    size_t got = fread (buffer, 1, 1024, stdin); 
    while (got > 0)
    {
        readScripts(fileName);
        got = fread (buffer, 1, 1024, stdin); 
    }
    fflush(stdout);
    return;
}

void addJob(pid_t pid, char * command){
    jobID++;
    jobList[jobID].jobID = jobID;
    jobList[jobID].commands = command;
    jobList[jobID].pid = pid;
    if (foregroundJob) { //pressed ctrl z
        foregroundJob = 0;
        jobList[jobID].isStopped = 1; 
        jobList[jobID].processStatus = "Stopped";
        printf("\n[%d]+ Stopped                 %s\n",jobList[jobID].jobID,jobList[jobID].commands); 
    } else {
        jobList[jobID].isStopped = 0; 
        jobList[jobID].processStatus = "Running";
        printf("[%d] %d\n", jobList[jobID].jobID,jobList[jobID].pid);       
    }
}

void updateJobList(){
    int status;
    pid_t pid;
    for (int i = 1; i <= jobID; i++){
        pid = waitpid(jobList[i].pid, &status, WNOHANG);
        if(pid > 0) {
            if (WIFEXITED(status)) {  
                jobList[i].processStatus = "Done"; 
            }
            printf("[%d]+ %s                 %s\n",i,jobList[i].processStatus,jobList[i].commands); 
        }
    }
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
    } else if (!pid){
    /* This is the child, so execute the ls */
        if (isRedir) { redir(args); }
        status = execvp (args[0], args);
        if (status < 0) {
            printf("bad command\n");
        }
        exit(1);
    } else {
        /* `
         * We're in the parent; let's wait for the child to finish
         */
        jobList[jobID].pid = getpid();
        //printf("%d parent1\n",jobList[jobID].pid);
        foregroundJob = 1;
        waitpid(pid, &status, 0);
    }
}

void printJobList(){
    char sign = '-';
    for (int i = 1; i <= jobID; i++)
    {          
        if(strcasecmp(jobList[i].processStatus,"Running") == 0) {
            printf("[%d]%c %s                 %s\n",i,sign,jobList[i].processStatus,jobList[i].commands); 
            sign = '+';
        } else if (strcasecmp(jobList[i].processStatus, "Stopped") == 0) {
            printf("[%d]+ %s                 %s\n",i,jobList[i].processStatus,jobList[i].commands); 
        }
    }
}

void fg(int id){
    if (id <= jobID && id > 0 && strcmp(jobList[id].processStatus,"Done")!=0) {
            int status;
            jobList[id].isStopped = 0;
            jobList[id].processStatus = "Running"; //back to work
            printf("[%d] %d\n",jobList[id].jobID,jobList[id].pid);
            kill(jobList[id].pid, SIGCONT); // resume the process
            foregroundJob = 1; //back to foreground
            waitpid(jobList[id].pid, &status, 0); // Wait for the process to finish
            externalRunning(toTokens(jobList[id].commands));
    } else {
        printf("Invalid job ID\n");
    }
}  

void bg(int id){
    if (id <= jobID && id > 0 && jobList[id].isStopped == 1) {
        jobList[id].isStopped = 0;
        jobList[id].processStatus = "Running"; //back to work
        kill(jobList[id].pid, SIGCONT); // resume the process
        foregroundJob = 0; //in background
        int status;
        pid_t pid = fork(); 
        if (pid < 0){
            perror ("Fork failed");
            exit(1);
        } else if (!pid){  //run in bg
            status = execvp (jobList[id].commands[0], jobList[id].commands);
            if (status < 0) {
                printf("bad command\n");
                exit(1);  
            }
        } else { //parent 
            printf("[%d]+ %s&\n",jobList[id].jobID,jobList[id].commands);
        }
    } else {
        printf("Invalid job ID\n");
    }
}


void command(char** current, char** prev) {
    int status;
    /* Turns prev to a string */
    char* prev_output = tokenStr(prev,1);
    char* current_input = tokenStr(current,0);
    /* !! */
    if (strcmp(current[0], "!!") == 0 && current[1] == NULL) {
        if (strcmp(prev_output, "") != 0) {
            printf("%s\n", prev_output);
        } else {
            printf("No previous command\n");
        }
    /* jobs */
    } else if (strcmp(current[0], "jobs") == 0 && current[1] == NULL) {
        printJobList();
    /* fg %<job_id> */
    } else if (strcmp(current[0], "fg") == 0 && current[1] != NULL && current[2] == NULL) {
        if (current[1][0] != '%') { 
            printf("Do you mean --> fg %%<job_id> ?\n");
        } else {
            char* removedSign = removeFirst(current[1]);
            int givenJobID = atoi(removedSign);
            fg(givenJobID);
        }
    /* bg %<job_id> */
    } else if (strcmp(current[0], "bg") == 0 && current[1] != NULL && current[2] == NULL) {
        if (current[1][0] != '%') { 
            printf("Do you mean --> bg %%<job_id> ?\n");
        } else {
            char* removedSign = removeFirst(current[1]);
            int givenJobID = atoi(removedSign);
            bg(givenJobID);
            free(removedSign);
        }
    /* echo */
    } else if (strcmp(current[0], "echo") == 0 && current[1] != NULL) {
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
        else if (current[2] == NULL) {
            printf("$ echo $?\n%s\n$\n",current[1]);
            int num = atoi(current[1]);
            if (num > 255) {
                num = num & 0xFF; //0b1111111
            }
                exit(num);
        }
    } else {
        int isJobBG = 0;
        int i = 0;
        while (current[i] != NULL) {
            if (strcmp(current[i], "<") == 0 || strcmp(current[i], ">") == 0) {
                isRedir = 1;
                break;
            } else if (strcmp(current[i], "&") == 0 ) {
                isJobBG = 1;
                current[i] = NULL; //remove last
                break;
            }
            i++;
        }
        if (isJobBG) {
            pid_t pid = fork();
            if (pid < 0){
                perror ("Fork failed");
                exit(1);
            } else if (!pid){ //child
                foregroundJob = 0;
                externalRunning(current);   
                exit(1);
            } else { //parent
                addJob(pid,tokenStr(current,0));
            }
        } else {
            foregroundJob = 1;
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
        prevBufferArr = toTokens(buffer); 
        while (fgets(buffer, MAX_LINE_LENGTH, file) != NULL) {
            curBufferArr = toTokens(buffer); 
            command(curBufferArr, prevBufferArr);
            prevBufferArr = copyTokens(curBufferArr);
            free(curBufferArr);
        }
        fclose(file);
    }
    return;
}

/* SIGNAL */
void signalHandler(int signum){
    pid_t pid = getpid();
    if (signum == SIGINT && foregroundJob) {
        foregroundJob = 0; //moved to bg
        kill(pid,SIGINT);
        printf("\nThe current foreground job KILLED\n");
    }
    if (signum == SIGTSTP && foregroundJob) {
        addJob(pid,tokenStr(curBufferArr,0));
        kill(pid, SIGTSTP);
        printf("The current foreground job SUSPENDED.\n");
    }
}

void signalHandlerSetUP(){
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = signalHandler;
    /* ctrl+C */
    sigaction(SIGINT, &sa, NULL);
    /* ctrl+Z */
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
        prevBufferArr = toTokens(buffer);
        printf("Starting IC SHELL\n");
        while (1) {
            printf("icsh $ ");
            fgets(buffer, MAX_CMD_BUFFER, stdin);
            curBufferArr = toTokens(buffer); 
            command(curBufferArr, prevBufferArr);
            prevBufferArr = copyTokens(curBufferArr);
            free(curBufferArr);        
            updateJobList();
        }
    }
    return 0;
}