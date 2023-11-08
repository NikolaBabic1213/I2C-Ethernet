#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define PORT 1492  // Port for communication
#define PWM_ADDRESS 0x40
#define MODE1_REGISTER 0x00
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];
    char ack[] = {0x00, 0x11, 0x22};

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(1);
    }

    printf("UDP server listening on port %d...\n", PORT);

    while (1) {
        // Receive data from the client
        int data_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
        int i = 0;
        printf("Received data: \n");
        for (; i < data_len; i++){
            printf("%02X\n", buffer[i]);
        }
        printf("\n");

        unsigned char rx_buffer = 0x00;
        char *reg;
        int length;
        int fd;
        if(buffer[0] == 0){
            reg = (char *)calloc(data_len - 2, sizeof(char));
            length = data_len - 2;
            for (i = 0; i < length + 1; i++){
                reg[i] = buffer[2 + i];
            }
        }
        struct i2c_msg iomsgs[] = {
            [0] = {
            .addr = buffer[1], /* slave address */
            .flags = 0, /* write access */
            .buf = &reg[0], /* register address */
            .len = length
            },
            [1] = {
            .addr = buffer[1], /* slave address */
            .flags = 0, /* write access */
            .buf = &buffer[2], /* register address */
            .len = 1
            },
            [2] = {
            .addr = buffer[1], /* slave address */
            .flags = I2C_M_RD, /* write access */
            .buf = &rx_buffer, /* register address */
            .len = 1
        }
        };

        struct i2c_rdwr_ioctl_data msgset[] = {
            [0] = {
                .msgs= &iomsgs[0],
                .nmsgs = 1
            },
            [1] = {
                .msgs = &iomsgs[1],
                .nmsgs = 2
            },
        };

        fd = open("/dev/i2c-1", O_RDWR);
        if (fd < 0) {
            printf("Error while trying to open i2c device.\n");
            return -1;
        }
        if (buffer[0] == 0){
            ioctl(fd, I2C_RDWR, &msgset[0]);
        }
        else{
            ioctl(fd, I2C_RDWR, &msgset[1]);
        }
        close(fd);
        if(buffer[0] == 0){
            free(reg);
        }
        sendto(sockfd, &rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *)&client_addr, addr_len);
   
    }
   

    close(sockfd);
    return 0;
}
