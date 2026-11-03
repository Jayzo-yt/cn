#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in receiver = {0}, sender;
    socklen_t len = sizeof(sender);
    char packet[100], ack[20], msg[80];
    int seq, expected = 0;

    srand(time(NULL));
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(7000);
    receiver.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(sock, (struct sockaddr *)&receiver, sizeof(receiver));
    printf("Receiver ready...\n");

    while (1) {
        int n = recvfrom(sock, packet, sizeof(packet) - 1, 0,
                 (struct sockaddr *)&sender, &len);
        packet[n] = '\0';
        sscanf(packet, "%d|%79s", &seq, msg);

        if (strcmp(msg, "END") == 0) {
            printf("Transmission finished\n");
            break;
        }

        if (rand() % 10 < 3) {
            printf("ACK lost for frame %d\n", seq);
            continue;
        }

        if (seq == expected) {
            printf("Received frame %d : %s\n", seq, msg);
            expected = 1 - expected;
        } else {
            printf("Duplicate frame %d - discarded\n", seq);
        }

        sprintf(ack, "%d", seq);
        sendto(sock, ack, strlen(ack), 0, (struct sockaddr *)&sender, len);
    }

    close(sock);
    return 0;
}
