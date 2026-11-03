#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server = {0};
    char msg[100], reply[100];

    server.sin_family = AF_INET;
    server.sin_port = htons(6000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
         printf("Message (bye to quit): ");
        fgets(msg, sizeof(msg), stdin);
         msg[strcspn(msg, "\n")] = '\0';
        sendto(sock, msg, strlen(msg), 0,
             (struct sockaddr *)&server, sizeof(server));
        if (strcmp(msg, "bye") == 0)
            break;

        int n = recvfrom(sock, reply, sizeof(reply) - 1, 0, NULL, NULL);
        reply[n] = '\0';
        printf("%s\n", reply);
    }

    close(sock);
    return 0;
}
