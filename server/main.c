#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../common/packet.h"

#define SERVER_IP "127.0.0.1"

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    Packet pkt;
    
    read(sock, &pkt, 1024);
    printf("[Client]: %s\n", pkt.data);
    char buffer[30] = "Hello from server";
    Packet response = { MSG_TYPE_CHAT, strlen(buffer), "" };
    strcpy(response.data,buffer);
    send(sock, &response, sizeof(response), 0);
    close(sock);
    free(socket_desc);
    return NULL;
}


int main() {
    int server_fd, *new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &address.sin_addr); 
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8081);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address))<0){
        perror("Failed to connect");
        exit(1);
    };
    if(listen(server_fd, 5)<0){
        perror("Failed to listen");
        exit(1);
    };

    printf("Server listening on port 8080...\n");

    while(1) {
        new_socket = malloc(sizeof(int));
        printf("Waiting for connection...\n");
        *new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        printf("New client connected...\n");

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void*)new_socket);
        pthread_detach(client_thread); // automatically clean up thread after finishing
    }

    close(server_fd);
    return 0;
}