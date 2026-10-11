
// Roll: 2410514257, Port: 30257

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 30257
#define SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server;
    socklen_t len = sizeof(server);
    char input[SIZE], message[SIZE], response[SIZE];

    // Check command-line arguments
    if (argc != 3) {
        printf("Usage: %s <server_ip> <roll>\n", argv[0]);
        return 1;
    }

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // Configure server address
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &server.sin_addr) != 1) {
        printf("Invalid server IP address\n");
        close(sockfd);
        return 1;
    }

    while (1) {
        printf("Enter text (or EXIT to quit): ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            strcpy(input, "EXIT");
        } else {
            input[strcspn(input, "\n")] = '\0';
        }

        // Add roll number prefix
        int written = snprintf(message, sizeof(message),
                               "%s#%s", argv[2], input);

        if (written < 0 || written >= SIZE) {
            printf("Request is too long\n");
            continue;
        }

        // Send request
        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&server, len) < 0) {
            perror("sendto");
            break;
        }

        // Receive server response
        int n = recvfrom(sockfd, response, SIZE - 1, 0,
                         NULL, NULL);

        if (n < 0) {
            perror("recvfrom");
            break;
        }

        response[n] = '\0';
        printf("Server response: %s\n", response);

        if (strcmp(input, "EXIT") == 0)
            break;
    }

    close(sockfd);
    return 0;
}