#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in receiver = {0};
    char ack[20], packet[100];
    char *frames[] = {"Hello", "This", "is", "Stop", "and", "Wait"};
    int seq = 0;

    struct timeval tv = {2, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(7000);
    receiver.sin_addr.s_addr = inet_addr("127.0.0.1");

    for (int i = 0; i < 6; i++) {
        while (1) {
            sprintf(packet, "%d|%s", seq, frames[i]);
            printf("Send: %s\n", packet);
            sendto(sock, packet, strlen(packet), 0,
                   (struct sockaddr *)&receiver, sizeof(receiver));
            int n = recvfrom(sock, ack, sizeof(ack) - 1, 0, NULL, NULL);
            if (n >= 0)
                ack[n] = '\0';
            if (n >= 0 && atoi(ack) == seq) {
                printf("ACK %d received\n\n", seq);
                seq = 1 - seq;
                break;
            }
            printf("Timeout: resend\n");
        }
    }

    sendto(sock, "0|END", 5, 0, (struct sockaddr *)&receiver, sizeof(receiver));
    close(sock);
    return 0;
}
