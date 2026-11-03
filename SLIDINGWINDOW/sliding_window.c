/* ==================== SENDER ==================== */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main()
{
    int s, total, window, mode, target;
    int base = 0, ack;
    int p[4];

    int fault = 0, duplicate = 0;

    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    s = socket(AF_INET, SOCK_DGRAM, 0);

    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,
               &tv, sizeof(tv));

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter total frames: ");
    scanf("%d", &total);

    printf("Enter window size: ");
    scanf("%d", &window);

    printf("\n0.Normal  1.Frame Loss  2.ACK Loss");
    printf("\n3.Corruption  4.Duplicate\n");

    printf("Enter choice: ");
    scanf("%d", &mode);

    target = -1;

    if (mode >= 1 && mode <= 4)
    {
        printf("Enter frame number for error: ");
        scanf("%d", &target);
    }

    while (base < total)
    {
        int end = base + window;

        if (end > total)
            end = total;

        printf("\nSending window: ");

        // Send frames
        for (int i = base; i < end; i++)
        {
            p[0] = i;
            p[1] = i + 100;
            p[2] = mode;
            p[3] = target;

            // Frame loss
            if (mode == 1 && i == target && fault == 0)
            {
                printf("\nFrame %d lost!\n", i);
                fault = 1;
                continue;
            }

            // Frame corruption
            if (mode == 3 && i == target && fault == 0)
            {
                p[1] = -1;
                printf("\nFrame %d corrupted!\n", i);
                fault = 1;
            }

            printf("%d ", i);

            sendto(s, p, sizeof(p), 0,
                   (struct sockaddr *)&server, len);

            // Duplicate frame
            if (mode == 4 && i == target && duplicate == 0)
            {
                printf("\nSending duplicate frame %d\n", i);

                sendto(s, p, sizeof(p), 0,
                       (struct sockaddr *)&server, len);

                duplicate = 1;
            }
        }

        printf("\nWaiting for ACK...\n");

        // Receive ACKs
        while (1)
        {
            int r = recvfrom(s, &ack, sizeof(ack), 0,
                             (struct sockaddr *)&server, &len);

            if (r <= 0)
                break;

            printf("Received ACK: %d\n", ack);

            if (ack >= base)
                base = ack + 1;

            if (base >= end)
                break;
        }

        if (base < end)
            printf("Timeout! Retransmitting unacknowledged frames...\n");
        else
            printf("Window moved!\n");
    }

    // Stop server
    p[0] = -1;

    sendto(s, p, sizeof(p), 0,
           (struct sockaddr *)&server, len);

    printf("\nTransmission completed!\n");

    close(s);
    return 0;
}

/* ==================== RECEIVER ==================== */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int s, n = 0, mode, target, ack, drop = 0;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    int p[4];

    s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));

    printf("Server waiting...\n");

    while (1)
    {
        recvfrom(s, p, sizeof(p), 0,
                 (struct sockaddr *)&client, &len);

        int frame = p[0];
        int data = p[1];
        mode = p[2];
        target = p[3];

        if (frame == -1)
            break;

        printf("\nReceived Frame: %d\n", frame);

        if (frame == n && data == frame + 100)
        {
            printf("Frame accepted\n");
            n++;
        }
        else
        {
            printf("Frame rejected / Duplicate frame\n");
        }

        ack = n - 1;

        // Simulate ACK loss
        if (mode == 2 && frame == target && drop == 0
            && ack == target)
        {
            printf("ACK %d lost!\n", ack);
            drop = 1;
            continue;
        }

        sendto(s, &ack, sizeof(ack), 0,
               (struct sockaddr *)&client, len);

        printf("ACK sent: %d\n", ack);
    }

    close(s);
    return 0;
}
