#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8000
#define BUFFER_SIZE 1024

int main() {
    int sockfd, n;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    // Replace "127.0.0.1" with the actual server IP address
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    // Open file for reading
    fp = fopen("sample.mp4", "rb");
    if (fp == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    // Send file data
    while ((n = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        write(sockfd, buffer, n);
    }

    // Send EOF marker
    char eof_marker = '\0';
    write(sockfd, &eof_marker, 1);

    // Receive success response
    char response[100];
    if (read(sockfd, response, sizeof(response)) < 0) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    printf("Server response: %s\n", response);

    // Close file and socket
    fclose(fp);
    close(sockfd);

    return 0;
}