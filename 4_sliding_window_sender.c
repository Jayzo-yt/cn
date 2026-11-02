#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main()
{
    int sock, base = 0, next = 0;
    int total = 5, window = 3;
    char buffer[100], packet[100];

    char *frames[] = {"F0", "F1", "F2", "F3", "F4"};

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // Set timeout of 2 seconds
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
               &tv, sizeof(tv));

    // Receiver address
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (base < total)
    {
        // Send frames within window
        while (next < base + window && next < total)
        {
            printf("Sending frame %d\n", next);

            sprintf(packet, "%d|%s", next, frames[next]);

            sendto(sock, packet, strlen(packet), 0,
                   (struct sockaddr *)&addr, len);

            next++;
        }

        // Receive ACK
        int n = recvfrom(sock, buffer, sizeof(buffer) - 1,
                         0, NULL, NULL);

        if (n < 0)
        {
            // Timeout: resend from base
            printf("Timeout! Resending from frame %d\n", base);

            next = base;
        }
        else
        {
            buffer[n] = '\0';

            int ack = atoi(buffer);

            printf("ACK received: %d\n", ack);

            // Move window forward
            if (ack >= base)
                base = ack + 1;
        }
    }

    // End transmission
    sendto(sock, "0|END", 5, 0,
           (struct sockaddr *)&addr, len);

    printf("All frames sent successfully\n");

    close(sock);

    return 0;
}