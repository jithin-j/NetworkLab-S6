#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server, client;

    if(argc != 2)
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
    server.sin_port = htons(atoi(argv[1]));

    if(bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("Error in binding\n");
        exit(1);
    }

    char buf[100];
    socklen_t len = sizeof(server);

    printf("Server waiting..\n");

    if(recvfrom(sock_fd, buf, 100, 0, (struct sockaddr *)&server, &len) == -1)
    {
        printf("Error in receiving\n");
        exit(1);
    }

    printf("Message got from client is : %s\n", buf);

    return 0;
}