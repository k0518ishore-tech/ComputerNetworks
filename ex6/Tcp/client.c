#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5555

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }
    printf("socket() created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return 1;
    }
    printf("connect() done\n");

    while (1) {
        n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';
        printf("%s", buffer);

        int len = strlen(buffer);
        if (strstr(buffer, "Username:") != NULL) {
            printf("Ask user name: ");
            fgets(buffer, sizeof(buffer), stdin);
            send(sock, buffer, strlen(buffer), 0);
        }
        else if (strstr(buffer, "Password:") != NULL) {
            printf("Ask password: ");
            fgets(buffer, sizeof(buffer), stdin);
            send(sock, buffer, strlen(buffer), 0);
        }
        else if (buffer[len - 1] == ':' || (len > 1 && buffer[len - 2] == ':')) {
            printf("> ");
            fgets(buffer, sizeof(buffer), stdin);
            send(sock, buffer, strlen(buffer), 0);
        }

        if (strstr(buffer, "Bye") != NULL || strstr(buffer, "Failed") != NULL) {
            break;
        }
    }

    close(sock);
    printf("closed\n");
    return 0;
}
