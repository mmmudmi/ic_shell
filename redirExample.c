#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void printHello() {
    printf("Hello\n");
}

int main() {
    int fileDescriptor = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fileDescriptor == -1) {
        perror("Failed to open file");
        return 1;
    }

    int originalStdout = dup(fileno(stdout));
    if (dup2(fileDescriptor, fileno(stdout)) == -1) {
        perror("Failed to redirect output");
        return 1;
    }

    // Call the function which will output "Hello" to the file
    printHello();

    // Restore the original stdout
    fflush(stdout);
    dup2(originalStdout, fileno(stdout));

    // Close the file descriptor
    close(fileDescriptor);

    return 0;
}
