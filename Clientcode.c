#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main(){
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock<0){
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        perror("Invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
        perror ("Connection failed");
        return -1;
    }

    while(1){
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "bye", 3)==0){
            printf("Disconnected from server.\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);

        read(sock, buffer, BUFFER_SIZE);
        printf("Server:%s", buffer);
    }

    close(sock);

    return 0;
}
    
