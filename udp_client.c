#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server, client;

    if(argc != 3)
    {
        printf("Input format not correct!\n");
        exit(1);
    }

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd == -1)
    {
        printf("Error in socket creation\n");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[2]));

    char buffer[100];
    printf("Enter message to be sent: \n");
    fgets(buffer, 100, stdin);

    if(sendto(sock_fd, buffer, 100, 0, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Error in sending\n");
        exit(1);
    }

    return 0;
}