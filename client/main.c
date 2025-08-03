#include <stdio.h>      
#include <stdlib.h> 
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <pthread.h> 
#include "../common/packet.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8081  

int sock;


void *receive_messages(void *arg) {
    Packet pkt;
    while (1) {
        int bytes = recv(sock, &pkt, sizeof(pkt), 0);
        if (bytes > 0) {
            printf("[Server]: %s\n", pkt.data);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr; 

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }
    printf("Socket connected...\n");

    serv_addr.sin_family = AF_INET;      
    serv_addr.sin_port = htons(PORT);  
    serv_addr.sin_addr.s_addr = INADDR_ANY;    


    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }


    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    char buffer[1024];
    while (1) {
        fgets(buffer, sizeof(buffer), stdin); 
        Packet pkt = { MSG_TYPE_CHAT, strlen(buffer), "" };
        strcpy(pkt.data, buffer);           
        send(sock, &pkt, sizeof(pkt), 0);
    }
}
