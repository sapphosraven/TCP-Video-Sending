#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8000
#define BUFFER_SIZE 1024

// Function to append "_received" to the base name of the file, before the extension
void get_received_filename(const char *filename, char *new_filename) {
    char *dot = strrchr(filename, '.'); // Find the last dot (file extension)
    if (dot != NULL) {
        // Copy base name (up to the dot)
        snprintf(new_filename, dot - filename + 1, "%s", filename);
        // Append "_received" and then the extension
        snprintf(new_filename + (dot - filename), 300 - (dot - filename), "_received%s", dot);
    } else {
        // No extension, just append "_received"
        snprintf(new_filename, 300, "%s_received", filename);
    }
}

int main() {
    int sockfd, newsockfd, valread;
    struct sockaddr_in servaddr, cliaddr;
    int opt = 1;
    int addrlen = sizeof(cliaddr);
    char buffer[BUFFER_SIZE];
    FILE *fp;
    char filename[256];
    char new_filename[300];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[ - ] socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket option for reuse
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ - ] setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("[ - ] bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 5) < 0) {
        perror("[ - ] listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Listening for incoming connections...\n");

    // Loop to keep the server listening for new connections
    while (1) {
        // Accept connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
        if (newsockfd < 0) {
            perror("[ - ] accept failed");
            continue;  // Keep listening for new connections even after an error
        }

        printf("[ + ] Client(Sender) connected\n");

        // Read the original file name from the sender
        memset(filename, 0, sizeof(filename)); // Clear filename buffer
        if (read(newsockfd, filename, sizeof(filename)) <= 0) {
            perror("[ - ] Failed to read filename");
            close(newsockfd);
            continue;  // Go back to listening for new connections
        }
        filename[strcspn(filename, "\n")] = 0;  // Remove newline if present

        // Generate new filename with "_received" appended
        get_received_filename(filename, new_filename);

        printf("[ + ] Receiving file and saving as: %s\n", new_filename);

        // Open file for writing
        fp = fopen(new_filename, "wb");
        if (fp == NULL) {
            perror("[ - ] fopen failed");
            close(newsockfd);
            continue;  // Go back to listening for new connections
        }

        // Receive file data
        while ((valread = read(newsockfd, buffer, BUFFER_SIZE)) > 0) {
            fwrite(buffer, 1, valread, fp);
        }

        // Check for completion
        if (valread == 0) {
            printf("[ + ] File received successfully\n");
        } else {
            perror("[ - ] read failed");
        }

        // Send success response
        char response[] = "File received successfully";
        if (write(newsockfd, response, strlen(response)) < 0) {
            perror("[ - ] write failed");
        }

        // Close file and socket
        fclose(fp);
        close(newsockfd);

        printf("[ + ] Connection closed. Ready to accept new connections.\nListening...\n\n");
    }

    close(sockfd);  // Close the server socket when the loop is broken (if needed)
    return 0;
}
