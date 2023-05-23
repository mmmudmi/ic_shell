/* ICCS227: Project 1: icsh
 * Name: Pearploy Chaicharoensin
 * StudentID: 6381278
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MAX_CMD_BUFFER 255

void readToken(char* Buffer) {
    char* buffer = strdup(Buffer); // copy
    char* token = strtok(buffer, " \t\n");
    token = strtok(NULL, " \t\n");
    while (token != NULL) {
        printf("%s ", token);
        token = strtok(NULL, " \t\n"); // update
    }
    free(buffer); // free 
}

char** toToken(char* buffer) {
    char** toReturn = malloc(MAX_CMD_BUFFER * sizeof(char*));
    char* token = strtok(buffer, " \t\n");
    int i = 0;
    while (token != NULL) {
        toReturn[i] = token;
        token = strtok(NULL, " \t\n"); // update
        i++;
    }
    toReturn[i] = NULL; // the end of an array
    return toReturn;
}

void printToken(char** token, int start) {
    for (int i = start; token[i] != NULL; i++) {
        printf("%s ", token[i]);
    }
}

int main() {
    char buffer[MAX_CMD_BUFFER];
    char prev_buffer[MAX_CMD_BUFFER];
    char prev_output[MAX_CMD_BUFFER] = ""; // initialize

    while (1) {
        printf("icsh $ ");
        fgets(buffer, sizeof(buffer), stdin); // take input to buffer

        if (strcmp(buffer, "!!\n") == 0) { 
            if (prev_output[0] != '\0') { // '\0' is like Null
                printf("%s %s\n", prev_buffer,prev_output);
                printf("%s\n", prev_output);
            } else {
                printf("No previous output\n");
            }
        } else {
            char** inputArr = toToken(buffer); 

            if (strcmp(inputArr[0], "echo") == 0) { 
                strcpy(prev_buffer, buffer);
                printToken(inputArr, 1);
                strcpy(prev_output, ""); // reset
                for (int i = 1; inputArr[i] != NULL; i++) {
                    strcat(prev_output, inputArr[i]);
                    strcat(prev_output, " ");
                }
                printf("\n");
            } else if (strcmp(inputArr[0], "exit") == 0) { 
                	    printf("bye\n");
                        int num = atoi(inputArr[1]);
                        if (num > 255) {
                            num = num & 0xFF; // Truncate the number to 8 bits
                        }
                        return num;
                //break; // exit
            } else {
                printf("bad command\n");
                strcpy(prev_output, ""); // reset
            }

            free(inputArr); // free 
        }
    }
    return 0;
}
