#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define WINDOW_SIZE 4

struct packet
{
    int seq_num;
    char data;
};

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    char *message = "Hello, Client!";
    int msglen = strlen(message);

    struct packet send_buffer[msglen];
    int send_buffer_size = 0;
    int base_seq_num = 0;

    while (base_seq_num < msglen)
    {
        char packet[1024] = {0};
        valread = read(new_socket, buffer, 1024);

        int seq_num = atoi(strtok(buffer, "|"));
        char data = strtok(NULL, "|")[0];
        printf("Received: Seq=%d, Data=%c\n", seq_num, data);

        struct packet pkt;
        pkt.seq_num = seq_num;
        pkt.data = data;

        if (pkt.seq_num >= base_seq_num && pkt.seq_num < base_seq_num + WINDOW_SIZE)
        {
            int index = pkt.seq_num - base_seq_num;
            send_buffer[index] = pkt;

            char ack[1024] = {0};
            sprintf(ack, "%d", pkt.seq_num);
            send(new_socket, ack, strlen(ack), 0);
            printf("Sent: Ack=%d\n", pkt.seq_num);
        }
        else
        {
            printf("Discarded: Seq=%d, Data=%c\n", pkt.seq_num, pkt.data);
        }

        int ack_received[WINDOW_SIZE] = {0};
        int packets_acked = 0;

        while (packets_acked < WINDOW_SIZE && base_seq_num < msglen)
        {
            char ack[1024] = {0};
            valread = read(new_socket, ack, 1024);

            int ack_seq_num = atoi(ack);
            printf("Received: Ack=%d\n", ack_seq_num);

            if (ack_seq_num >= base_seq_num && ack_seq_num < base_seq_num + WINDOW_SIZE)
            {
                int index = ack_seq_num - base_seq_num;
                if (!ack_received[index])
                {
                    ack_received[index] = 1;
                    packets_acked++;
                }

                if (index == 0)
                {
                    while (ack_received[0] && base_seq_num < msglen)
                    {
                        for (int i = 0; i < WINDOW_SIZE; i++)
                        {
                            send_buffer[i] = send_buffer[i + 1];
                        }

                        base_seq_num++;

                        if (base_seq_num < msglen)
                        {
                            struct packet next_pkt = send_buffer[WINDOW_SIZE - 1];
                            char next_packet[1024] = {0};
                            sprintf(next_packet, "%d|%c", next_pkt.seq_num, next_pkt.data);
                            send(new_socket, next_packet, strlen(next_packet), 0);
                            printf("Sent: Seq=%d, Data=%c\n", next_pkt.seq_num, next_pkt.data);
                        }
                    }
                }
            }
            else
            {
                printf("Discarded: Ack=%d\n", ack_seq_num);
            }
        }
    }

    printf("Message received: ");

    for (int i = 0; i < send_buffer_size; i++)
    {
        printf("%c", send_buffer[i].data);
    }

    printf("\n");

    return 0;
}