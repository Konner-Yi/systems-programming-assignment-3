//Group 16 Assignment 3
//Konner Yi

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT 9001
#define BUF_SIZE 1024

void *client_handler(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);

    char buffer[BUF_SIZE];
    ssize_t bytes;

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        bytes = recv(client_sock, buffer, BUF_SIZE - 1, 0);

        if (bytes <= 0) {
            printf("Client disconnected.\n");
            close(client_sock);
            pthread_exit(NULL);
        }

        buffer[strcspn(buffer, "\r\n")] = 0;  // remove newline

        if (strcmp(buffer, "TIME") == 0) {
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
            send(client_sock, time_str, strlen(time_str), 0);

        } else if (strcmp(buffer, "DATE") == 0) {
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            char date_str[20];
            strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
            send(client_sock, date_str, strlen(date_str), 0);

        } else if (strcmp(buffer, "EXIT") == 0) {
            char goodbye[] = "Goodbye";
            send(client_sock, goodbye, strlen(goodbye), 0);
            close(client_sock);
            pthread_exit(NULL);

        } else {
            char invalid[] = "Invalid command";
            send(client_sock, invalid, strlen(invalid), 0);
        }
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket()");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind()");
        exit(1);
    }

    if (listen(server_sock, 5) < 0) {
        perror("listen()");
        exit(1);
    }

    printf("Server running on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addrlen);
        if (client_sock < 0) {
            perror("accept()");
            continue;
        }

        printf("Client connected.\n");

        pthread_t thread;
        int *pclient = malloc(sizeof(int));
        *pclient = client_sock;

        pthread_create(&thread, NULL, client_handler, pclient);
        pthread_detach(thread); // allow thread to clean itself up
    }

    close(server_sock);
    return 0;
}
