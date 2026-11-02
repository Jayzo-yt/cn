// UDP Server
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);
    char buffer[1024];

    sock = socket(AF_INET, SOCK_DGRAM, 0);            // UDP socket

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(6000);

    bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("UDP Server ready...\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                 (struct sockaddr *)&client_addr, &len);   // no connection needed
        if (strcmp(buffer, "bye") == 0)
            break;
        printf("Client: %s\n", buffer);

        char reply[1100];
        sprintf(reply, "Server received: %s", buffer);
        sendto(sock, reply, strlen(reply), 0,
               (struct sockaddr *)&client_addr, len);
    }

    close(sock);
    return 0;
}
