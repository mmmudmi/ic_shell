#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CMD_BUFFER 255
#define MAX_LINE_LENGTH 1000

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
            printf("%s %s\n", prev[0], prev_output);
            printf("%s\n", prev_output);
        } else {
            printf("No previous output\n");
        }
    } else {
        /* echo */
        if (strcmp(current[0], "echo") == 0 && current[1] != NULL) {
            printToken(current, 1);
        /* exit */
        } else if (strcmp(current[0], "exit") == 0 && current[2] == NULL) {
            printf("bye\n");
            int num = atoi(current[0]);
            if (num > 255) {
                num = num & 0xFF;
            }
            exit(num);
        } else {
            printf("bad command\n");
        }
    }

    free(prev_output);
}

int main() {
    char buffer[MAX_CMD_BUFFER];
    char** prevBufferArr = toTokens(buffer); //["echo","hi"] 
    printf("Starting IC shell\n");

    while (1) {
        printf("icsh $ ");
        fgets(buffer, MAX_CMD_BUFFER, stdin);
        char** curBufferArr = toTokens(buffer); 
        command(curBufferArr, prevBufferArr);
        prevBufferArr = copyTokens(curBufferArr);
    }

    return 0;
}
