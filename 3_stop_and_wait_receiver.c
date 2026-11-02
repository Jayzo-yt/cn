// Stop-and-Wait ARQ - Receiver (run this FIRST)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in my_addr, sender_addr;
    socklen_t len = sizeof(sender_addr);
    char buffer[1024], msg[1000], ack[20];
    int seq, expected = 0;

    srand(time(NULL));
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    my_addr.sin_port = htons(7000);
    bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr));
    printf("Receiver ready...\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                 (struct sockaddr *)&sender_addr, &len);
        sscanf(buffer, "%d|%s", &seq, msg);            // format: seq|data

        if (strcmp(msg, "END") == 0) {
            printf("Transmission finished\n");
            break;
        }

        // Simulate ACK loss: 30% of the time, do not reply
        if (rand() % 10 < 3) {
            printf("Frame %d received but ACK lost (simulated)\n", seq);
            continue;
        }

        if (seq == expected) {
            printf("Received frame %d : %s\n", seq, msg);
            expected = 1 - expected;                   // toggle 0 <-> 1
        } else {
            printf("Duplicate frame %d - discarded\n", seq);
        }

        sprintf(ack, "%d", seq);
        sendto(sock, ack, strlen(ack), 0,
               (struct sockaddr *)&sender_addr, len);  // send ACK
    }

    close(sock);
    return 0;
}
