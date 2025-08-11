#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include "../common/packet.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8081
#define MAX_CLIENTS 10

int* clients[MAX_CLIENTS] = {0};
int next_client_slot = 0;

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    Packet pkt;
    ssize_t bytes_read;
    
    printf("Client thread started for socket %d\n", sock);
    
    // Keep connection alive and handle multiple messages
    while (1) {
        // Read packet with proper error handling
        bytes_read = recv(sock, &pkt, sizeof(Packet), 0);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                printf("Client disconnected gracefully\n");
            } else {
                perror("Error reading from client");
            }
            break; // Exit the message loop
        }
        
        printf("[Client %d]: %s\n", sock, pkt.data);
        
        // Prepare response
        char buffer[1024] = "[MSG Received]";
        Packet response = { MSG_TYPE_CHAT, strlen(buffer), "" };
        strncpy(response.data, buffer, sizeof(response.data) - 1);
        response.data[sizeof(response.data) - 1] = '\0'; // Ensure null termination
        
        // Send response with error handling
        if (send(sock, &response, sizeof(response), 0) < 0) {
            perror("Failed to send response");
            break; // Exit if we can't send
        }
        
    }
    
    // Clean up when client disconnects
    printf("Client thread ending for socket %d\n", sock);
    close(sock);
    free(socket_desc);
    return NULL;
}

int main() {
    int server_fd, *new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(1);
    }
    
    // Configure address
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    
    // Listen for connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(1);
    }
    
    printf("Server listening on port %d...\n", SERVER_PORT);
    
    while(1) {
        new_socket = malloc(sizeof(int));
        if (!new_socket) {
            perror("Memory allocation failed");
            continue;
        }
        
        *new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        
        if (*new_socket < 0) {
            perror("Accept failed");
            free(new_socket);
            continue;
        }
        
        printf("New client connected...\n");
        
        // Find available slot for client
        int client_slot = -1;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == 0) {
                client_slot = i;
                break;
            }
        }
        
        if (client_slot == -1) {
            printf("Client capacity reached\n");
            close(*new_socket);
            free(new_socket);
            continue;
        }
        
        // Store client socket
        clients[client_slot] = new_socket;
        
        // Create thread to handle client
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void*)new_socket) != 0) {
            perror("Failed to create client thread");
            clients[client_slot] = 0;
            close(*new_socket);
            free(new_socket);
            continue;
        }
        
        pthread_detach(client_thread);
    }
    
    close(server_fd);
    return 0;
}