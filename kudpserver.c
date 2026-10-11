// Roll: 2410514257 Service Type: C d = 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 30257
#define ROLL "2410514257"
#define SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t len;
    char buffer[SIZE], response[SIZE];

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // Configure server address
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *)&server,
             sizeof(server)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("Server running on port %d\n", PORT);

    while (1) {
        len = sizeof(client);

        int n = recvfrom(sockfd, buffer, SIZE - 1, 0,
                         (struct sockaddr *)&client, &len);

        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[n] = '\0';

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));

        printf("\nClient address: %s:%d\n",
               ip, ntohs(client.sin_port));

        // Validate roll number prefix
        char *sep = strchr(buffer, '#');

        if (sep == NULL) {
            strcpy(response, "ERROR: Unauthorized client");
        } else {
            *sep = '\0';

            if (strcmp(buffer, ROLL) != 0) {
                strcpy(response, "ERROR: Unauthorized client");
            } else {
                char *msg = sep + 1;

                printf("Received message: %s\n", msg);

                // Exit command
                if (strcmp(msg, "EXIT") == 0) {
                    strcpy(response, "Server shutting down");

                    sendto(sockfd, response, strlen(response), 0,
                           (struct sockaddr *)&client, len);

                    printf("Sent response: %s\n", response);
                    break;
                }

                // Calculate string statistics
                int length = strlen(msg);
                int vowels = 0;
                int consonants = 0;
                int digits = 0;

                for (int i = 0; msg[i] != '\0'; i++) {
                    unsigned char ch = (unsigned char)msg[i];

                    if (isalpha(ch)) {
                        char c = (char)tolower(ch);

                        if (c == 'a' || c == 'e' ||
                            c == 'i' || c == 'o' ||
                            c == 'u') {
                            vowels++;
                        } else {
                            consonants++;
                        }
                    } else if (isdigit(ch)) {
                        digits++;
                    }
                }

                snprintf(response, sizeof(response),
                         "LEN=%d;VOWELS=%d;CONSONANTS=%d;DIGITS=%d",
                         length, vowels, consonants, digits);
            }
        }

        // Send response to client
        if (sendto(sockfd, response, strlen(response), 0,
                   (struct sockaddr *)&client, len) < 0) {
            perror("sendto");
            continue;
        }

        printf("Sent response: %s\n", response);
    }

    close(sockfd);
    return 0;
}