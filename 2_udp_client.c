// UDP Client
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(server_addr);
    char buffer[1024], msg[1024];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(6000);

    while (1) {
        printf("Enter message (bye to quit): ");
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = 0;

        sendto(sock, msg, strlen(msg), 0,
               (struct sockaddr *)&server_addr, len);
        if (strcmp(msg, "bye") == 0)
            break;

        memset(buffer, 0, sizeof(buffer));
        recvfrom(sock, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
        printf("%s\n", buffer);
    }

    close(sock);
    return 0;
}
