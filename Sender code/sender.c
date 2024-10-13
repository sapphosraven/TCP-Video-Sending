#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8000
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("[ - ] Usage: %s <file_to_send>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd, n;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[ - ] socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        perror("[ - ] inet_pton error");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("[ - ] connect failed");
        exit(EXIT_FAILURE);
    }
    printf("[ + ] Connected to server\n");

    // Send filename to the server
    if (write(sockfd, argv[1], strlen(argv[1])) < 0) {
        perror("[ - ] Failed to send filename");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Open file for reading
    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("[ - ] fopen failed");
        exit(EXIT_FAILURE);
    }

    // Send file data
    printf("[ + ] Sending file: %s\n", argv[1]);
    while ((n = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        if (write(sockfd, buffer, n) < 0) {
            perror("[ - ] write failed");
            fclose(fp);
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    // Close the file and signal EOF
    fclose(fp);
    shutdown(sockfd, SHUT_WR);
    printf("[ + ] File sent successfully\n");

    // Receive success response
    char response[100] = {0};  // Ensure buffer is zeroed out to prevent garbage values
    if ((n = read(sockfd, response, sizeof(response) - 1)) < 0) {
        perror("[ - ] read failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    response[n] = '\0';  // Properly null-terminate the string
    printf("[ + ] Server(Receiver) response: %s\n", response);

    // Close socket
    close(sockfd);
    printf("[ + ] Connection closed\n\n");

    return 0;
}
