#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sock, seq;
    int expected = 0;

    char buffer[100], msg[100], ack[20];

    struct sockaddr_in addr, sender;
    socklen_t len = sizeof(sender);

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // Receiver address
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind socket
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    printf("Receiver waiting...\n");

    while (1)
    {
        // Receive frame
        int n = recvfrom(sock, buffer, sizeof(buffer) - 1,
                         0, (struct sockaddr *)&sender, &len);

        buffer[n] = '\0';

        // Separate sequence number and message
        sscanf(buffer, "%d|%s", &seq, msg);

        // Check END message
        if (strcmp(msg, "END") == 0)
        {
            printf("Transmission completed\n");
            break;
        }

        // Check whether frame is in order
        if (seq == expected)
        {
            printf("Received frame %d : %s\n", seq, msg);

            expected++;
        }
        else
        {
            printf("Frame %d discarded\n", seq);
        }

        // Send cumulative ACK
        sprintf(ack, "%d", expected - 1);

        sendto(sock, ack, strlen(ack), 0,
               (struct sockaddr *)&sender, len);
    }

    close(sock);

    return 0;
}