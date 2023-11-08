#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define SERVER_IP "192.168.100.101"  
#define SERVER_PORT 1492  

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

   
    while (1){
        int n = 0;
        char temp;
       
        printf("Write(0)/Read(1): ");
        scanf("%x", &temp);

        if (temp == 0x00){
                printf("Number of messages (enter 0 to exit): ");
                scanf("%d", &n);
                if(n <= 0){
                    close(sockfd);
                    return 0;
                }
        }
       
        char message[3 + n];
        message[0] = temp;
        printf("Slave address: ");
        scanf("%x", &message[1]);
        printf("Register address: ");
        scanf("%x", &message[2]);
        int i;
        if (message[0] == 0x00){
            for (i = 0; i < n; i++){
                printf("Message number %d: ", i + 1);
                scanf("%x", &message[3 + i]);
            }
        }
        ssize_t bytes_sent = sendto(sockfd, message, sizeof(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bytes_sent == -1) {
            perror("Sendto failed");
        } else {
            printf("Sent: ");
            for (i = 0; i < bytes_sent; i++){
                printf("%02x, ", message[i]);
            }
            printf("\n\n");
        }
        ssize_t data_len = recvfrom(sockfd, buffer, 1024, 0, NULL, NULL);
        if (message[0] == 0x00){
            if (data_len){
                printf("Message successfully written.\n");
            }
            else{
                printf("Message has not been written.\n");
            }
        }
        else{
            printf("\nData in register %x: ", message[2]);
            printf("%02x\n", buffer[0]);
        }
        printf("\n");
    }
    close(sockfd);
    return 0;
}
