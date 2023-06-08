#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080
#define WINDOW_SIZE 4
#define TIMEOUT 3

struct packet {
    int seq_num;
    char data;
};

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    char *message = "Hello, Server!";
    int msglen = strlen(message);

    struct packet send_buffer[WINDOW_SIZE];
    int send_buffer_size = 0;
    int next_seq_num = 0;
    int base_seq_num = 0;
    int ack_received[WINDOW_SIZE] = {0};

    srand(time(NULL));

    while (base_seq_num < msglen) {
        while (next_seq_num < base_seq_num + WINDOW_SIZE && next_seq_num < msglen) {
            struct packet pkt;
            pkt.seq_num = next_seq_num;
            pkt.data = message[next_seq_num];

            send_buffer[send_buffer_size++] = pkt;
            printf("Sent: Seq=%d, Data=%c\n", pkt.seq_num, pkt.data);

            next_seq_num++;
        }

        for (int i = base_seq_num; i < send_buffer_size; i++) {
            if (!ack_received[i]) {
                struct packet pkt = send_buffer[i];
                char packet[1024];
                sprintf(packet, "%d|%c", pkt.seq_num, pkt.data);

                if (rand() % 10 < 4) { // Simulate packet loss
                    printf("Dropped: Seq=%d, Data=%c\n", pkt.seq_num, pkt.data);
                    continue;
                }

                send(sock, packet, strlen(packet), 0);
                printf("Sent: Seq=%d, Data=%c\n", pkt.seq_num, pkt.data);
            }
        }

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        struct timeval timeout;
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        int activity = select(sock + 1, &readfds, NULL, NULL, &timeout);
        if (activity == -1) {
            perror("select");
            return -1;
        } else if (activity == 0) {
            printf("Timeout occurred, resending unacknowledged packets...\n");
            for (int i = base_seq_num; i < send_buffer_size; i++) {
                if (!ack_received[i]) {
                    struct packet pkt = send_buffer[i];
                    char packet[1024];
                    sprintf(packet, "%d|%c", pkt.seq_num, pkt.data);

                    if (rand() % 10 < 4) { //
                    printf("Dropped: Seq=%d, Data=%c\n", pkt.seq_num, pkt.data);
                    continue;
                }

                send(sock, packet, strlen(packet), 0);
                printf("Resent: Seq=%d, Data=%c\n", pkt.seq_num, pkt.data);
            }
        }
    } else {
        char ack[1024] = {0};
        valread = read(sock, ack, 1024);

        int ack_seq_num = atoi(ack);
        printf("Received: Ack=%d\n", ack_seq_num);

        for (int i = base_seq_num; i < send_buffer_size; i++) {
            if (!ack_received[i]) {
                if (send_buffer[i].seq_num == ack_seq_num) {
                    ack_received[i] = 1;

                    if (i == base_seq_num) {
                        while (ack_received[base_seq_num] && base_seq_num < send_buffer_size) {
                            base_seq_num++;
                        }
                    }

                    break;
                }
            }
        }
    }
}

return 0;
}

