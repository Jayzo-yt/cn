#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0), seq;
    int expected = 0;

    char buffer[100], msg[100], ack[20];

    struct sockaddr_in addr = {0}, sender;
    socklen_t len = sizeof(sender);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    printf("Receiver waiting...\n");

    while (1)
    {
        int n = recvfrom(sock, buffer, sizeof(buffer) - 1,
                         0, (struct sockaddr *)&sender, &len);
        if (n < 0)
            continue;
        buffer[n] = '\0';
        sscanf(buffer, "%d|%99s", &seq, msg);

        if (strcmp(msg, "END") == 0)
        {
            printf("Transmission completed\n");
            break;
        }

        if (seq == expected)
        {
            printf("Received frame %d : %s\n", seq, msg);

            expected++;
        }
        else
        {
            printf("Frame %d discarded\n", seq);
        }

        sprintf(ack, "%d", expected - 1);
        sendto(sock, ack, strlen(ack), 0, (struct sockaddr *)&sender, len);
    }

    close(sock);

    return 0;
}