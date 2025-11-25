//Group 16 Assignment 3
//Konner Yi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9001
#define BUF_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket()");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(1);
    }

    printf("Connected to server.\n");

    while (1) {
        printf("Enter command (TIME, DATE, EXIT): ");
        fgets(buffer, BUF_SIZE, stdin);
        buffer[strcspn(buffer, "\r\n")] = 0;

        send(sock, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUF_SIZE);
        recv(sock, buffer, BUF_SIZE - 1, 0);

        printf("Server: %s\n", buffer);

        if (strcmp(buffer, "Goodbye") == 0)
            break;
    }

    close(sock);
    return 0;
}
