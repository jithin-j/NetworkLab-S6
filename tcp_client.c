#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
    int k;
    int sock_desc;
    char buf[100];
    struct sockaddr_in client;

    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1)
    {
        printf("Error in socket creation");
        exit(1);
    }

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY;
    client.sin_port = 3003;

    k = connect(sock_desc, (struct sockaddr *)&client, sizeof(client));
    if (k == -1)
    {
        printf("Error in connecting to server");
        exit(1);
    }

    printf("Enter message to send to server:\n");
    fgets(buf, 100, stdin);

    k = send(sock_desc, buf, 100, 0);
    if (k == -1)
    {
        printf("Error in sending");
        exit(1);
    }

    close(sock_desc);
    return 0;
}