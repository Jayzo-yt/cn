#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server = {0}, client;
    socklen_t len = sizeof(client);
    char msg[100], reply[120];

    server.sin_family = AF_INET;
    server.sin_port = htons(6000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(sock, (struct sockaddr *)&server, sizeof(server));
    printf("UDP Server ready...\n");

    while (1) {
         int n = recvfrom(sock, msg, sizeof(msg) - 1, 0,
                    (struct sockaddr *)&client, &len);
         msg[n] = '\0';
         if (strcmp(msg, "bye") == 0)
            break;
         printf("Client: %s\n", msg);
         snprintf(reply, sizeof(reply), "Server received: %s", msg);
        sendto(sock, reply, strlen(reply), 0,
             (struct sockaddr *)&client, len);
    }

    close(sock);
    return 0;
}
