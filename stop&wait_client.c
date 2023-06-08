#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    char *message = "Hello, Server!";
    int msglen = strlen(message);

    int seq_num = 0;
    while (seq_num < msglen)
    {
        char seq[2];
        sprintf(seq, "%d", seq_num);

        char packet[1024];
        strcpy(packet, seq);
        strcat(packet, "|");
        strncat(packet, message + seq_num, 1);

        send(sock, packet, strlen(packet), 0);
        printf("Sent: %s\n", packet);

        char ack[1024] = {0};
        valread = read(sock, ack, 1024);
        printf("Received Ack: %s\n", ack);

        if (ack[0] == seq[0])
        {
            seq_num++;
        }
    }

    return 0;
}