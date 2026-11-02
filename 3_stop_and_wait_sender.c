// Stop-and-Wait ARQ - Sender
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main() {
    int sock;
    struct sockaddr_in receiver_addr;
    socklen_t len = sizeof(receiver_addr);
    char buffer[1024], pkt[1024];
    char *frames[] = {"Hello", "This", "is", "Stop", "and", "Wait"};
    int total = 6, seq = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // Timeout of 2 seconds for recvfrom
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    receiver_addr.sin_port = htons(7000);

    for (int i = 0; i < total; i++) {
        while (1) {
            printf("Sending frame %d : %s\n", seq, frames[i]);
            sprintf(pkt, "%d|%s", seq, frames[i]);
            sendto(sock, pkt, strlen(pkt), 0,
                   (struct sockaddr *)&receiver_addr, len);

            memset(buffer, 0, sizeof(buffer));
            int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, NULL, NULL);

            if (n < 0) {                                // timeout
                printf("Timeout! Resending frame %d\n\n", seq);
            } else if (atoi(buffer) == seq) {
                printf("ACK %d received\n\n", seq);
                seq = 1 - seq;                          // toggle 0 <-> 1
                break;
            }
        }
    }

    sendto(sock, "0|END", 5, 0, (struct sockaddr *)&receiver_addr, len);
    close(sock);
    return 0;
}
