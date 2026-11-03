/* ==================== SENDER ==================== */


#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>

struct Packet {
    int frame, seq, data, mode, target;
};

struct Ack {
    int frame, seq;
};

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, 0);

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,
               &tv, sizeof(tv));

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    int len = sizeof(server);
    int n, mode, target = -1;

    printf("Enter number of frames: ");
    scanf("%d", &n);

    printf("\n0.Normal\n1.Frame Loss\n2.ACK Loss");
    printf("\n3.Corruption\n4.Duplicate\n");

    printf("Enter mode: ");
    scanf("%d", &mode);

    if (mode != 0) {
        printf("Enter target frame: ");
        scanf("%d", &target);
    }

    struct Packet p;
    struct Ack ack;

    for (int i = 0; i < n; i++) {

        p.frame = i;
        p.seq = i % 2;
        p.data = i + 100;
        p.mode = mode;
        p.target = target;

        int attempt = 0;

        while (1) {

            if (attempt == 0 &&
                mode == 1 && i == target) {

                printf("\nFrame %d Lost!\n", i);
            }

            else {
                if (attempt == 0 &&
                    mode == 3 && i == target) {

                    p.data = -1;
                    printf("\nSending Corrupted Frame\n");
                }

                else {
                    p.data = i + 100;
                }

                printf("\nSending Frame: %d\n", i);

                sendto(s, &p, sizeof(p), 0,
                       (struct sockaddr *)&server,
                       sizeof(server));

                if (attempt == 0 &&
                    mode == 4 && i == target) {

                    printf("Sending Duplicate Frame\n");

                    sendto(s, &p, sizeof(p), 0,
                           (struct sockaddr *)&server,
                           sizeof(server));
                }
            }

            int r = recvfrom(s, &ack, sizeof(ack), 0,
                             (struct sockaddr *)&server,
                             (socklen_t *)&len);

            if (r > 0) {
                if (ack.frame == i && ack.seq == p.seq) {
                    printf("ACK Received: %d\n", ack.frame);
                    break;
                }

                printf("Old or Unexpected ACK Ignored\n");
            }

            else if (r < 0 &&
                     (errno == EAGAIN || errno == EWOULDBLOCK)) {

                printf("Timeout! Retransmitting Frame %d\n", i);
            }

            else {
                perror("recvfrom");
                close(s);
                return 1;
            }

            attempt++;
        }
    }

    p.frame = -1;

    sendto(s, &p, sizeof(p), 0,
           (struct sockaddr *)&server, sizeof(server));

    close(s);
    return 0;
}

/* ==================== RECEIVER ==================== */


#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

struct Packet {
    int frame, seq, data, mode, target;
};

struct Ack {
    int frame, seq;
};

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));

    int expected = 0;
    int expectedSeq = 0;
    int dropped = 0;

    struct Packet p;
    struct Ack ack;

    while (1) {
        recvfrom(s, &p, sizeof(p), 0,
                 (struct sockaddr *)&client, &len);

        if (p.frame == -1)
            break;

        printf("\nReceived Frame: %d\n", p.frame);

        if (p.frame == expected &&
            p.seq == expectedSeq) {

            if (p.data != p.frame + 100) {
                printf("Corrupted Frame!\n");
                continue;
            }

            printf("Frame Accepted\n");

            ack.frame = p.frame;
            ack.seq = p.seq;

            expected++;
            expectedSeq = 1 - expectedSeq;

            if (p.mode == 2 &&
                p.frame == p.target && !dropped) {

                printf("ACK Lost!\n");
                dropped = 1;
                continue;
            }

            sendto(s, &ack, sizeof(ack), 0,
                   (struct sockaddr *)&client, len);

            printf("ACK Sent: %d\n", ack.frame);
        }

        else if (p.frame < expected) {
            printf("Duplicate Frame!\n");

            ack.frame = p.frame;
            ack.seq = p.seq;

            sendto(s, &ack, sizeof(ack), 0,
                   (struct sockaddr *)&client, len);

            printf("ACK Resent: %d\n", ack.frame);
        }
    }

    close(s);
    return 0;
}
