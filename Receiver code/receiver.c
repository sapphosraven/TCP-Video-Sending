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
    int sockfd, newsockfd, valread;
    struct sockaddr_in servaddr, cliaddr;
    int opt = 1;
    int addrlen = sizeof(cliaddr);
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket option for reuse
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept connection
    newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
    if (newsockfd < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Open file for writing
    fp = fopen("received_sample.mp4", "wb");
    if (fp == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    // Receive file data
    while ((valread = read(newsockfd, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, valread, fp);
    }

    // Check for EOF marker
    if (valread == 0) {
        printf("EOF received\n");
    } else {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    // Send success response
    char response[] = "File received successfully";
    if (write(newsockfd, response, strlen(response)) < 0) {
        perror("write failed");
        exit(EXIT_FAILURE);
    }

    // Close file and socket
    fclose(fp);
    close(newsockfd);
    close(sockfd);

    return 0;
}