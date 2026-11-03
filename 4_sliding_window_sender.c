#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    int base = 0, next = 0, total = 5, window = 3;
    char buffer[100], packet[100];

    char *frames[] = {"F0", "F1", "F2", "F3", "F4"};

    struct sockaddr_in addr = {0};

    struct timeval tv = {2, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
               &tv, sizeof(tv));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (base < total)
    {
        while (next < base + window && next < total)
        {
            sprintf(packet, "%d|%s", next, frames[next]);
            printf("Send: %s\n", packet);
            sendto(sock, packet, strlen(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
            next++;
        }

        int n = recvfrom(sock, buffer, sizeof(buffer) - 1,
                         0, NULL, NULL);

        if (n < 0)
        {
            printf("Timeout: resend from %d\n", base);
            next = base;
        }
        else
        {
            buffer[n] = '\0';

            int ack = atoi(buffer);

            printf("ACK received: %d\n", ack);

            if (ack >= base)
                base = ack + 1;
        }
    }

    sendto(sock, "0|END", 5, 0, (struct sockaddr *)&addr, sizeof(addr));

    printf("All frames sent successfully\n");

    close(sock);

    return 0;
}