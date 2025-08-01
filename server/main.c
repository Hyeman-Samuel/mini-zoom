#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[1024] = {0};
    
    read(sock, buffer, 1024);
    printf("Client says: %s\n", buffer);
    send(sock, "Hello from server", 17, 0);
    
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
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    printf("Server listening on port 8080...\n");

    while(1) {
        new_socket = malloc(sizeof(int));
        *new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        printf("New client connected...\n");

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void*)new_socket);
        pthread_detach(client_thread); // automatically clean up thread after finishing
    }

    close(server_fd);
    return 0;
}