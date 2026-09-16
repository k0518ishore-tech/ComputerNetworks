#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5555

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];
    int n;

    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }
    char users[5][50];
    char passes[5][50];
    int count = 0;
    while (fscanf(fp, "%49[^:]:%49[^\n]\n", users[count], passes[count]) == 2) {
        count++;
    }
    fclose(fp);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }
    printf("socket() created\n");

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 1;
    }
    printf("bind() done on port %d\n", PORT);

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        return 1;
    }
    printf("listen() active\n");

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("accept");
        return 1;
    }
    printf("accept() connected\n");

    send(client_fd, "Welcome\nUsername: ", 18, 0);
    n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        close(client_fd);
        close(server_fd);
        return 0;
    }
    buffer[n] = '\0';
    buffer[strcspn(buffer, "\n")] = '\0';
    char user[50];
    strcpy(user, buffer);

    send(client_fd, "Password: ", 10, 0);
    n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        close(client_fd);
        close(server_fd);
        return 0;
    }
    buffer[n] = '\0';
    buffer[strcspn(buffer, "\n")] = '\0';
    char pass[50];
    strcpy(pass, buffer);

    int auth = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i], user) == 0 && strcmp(passes[i], pass) == 0) {
            auth = 1;
            break;
        }
    }

    if (auth == 0) {
        send(client_fd, "Failed\n", 7, 0);
        close(client_fd);
        close(server_fd);
        return 0;
    }

    send(client_fd, "Success\n", 8, 0);

    while (1) {
        send(client_fd, "\n--- Menu ---\n1. Echo\n2. Palindrome\n3. Exit\nChoice: ", 52, 0);
        n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "1") == 0) {
            send(client_fd, "Message: ", 9, 0);
            n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (n <= 0) break;
            buffer[n] = '\0';
            buffer[strcspn(buffer, "\n")] = '\0';

            printf("Received message: %s\n", buffer);
            printf("Message received\n");

            char reply[1100];
            sprintf(reply, "ECHO: %s\n", buffer);
            printf("Respond msg send\n");
            send(client_fd, reply, strlen(reply), 0);
        }
        else if (strcmp(buffer, "2") == 0) {
            send(client_fd, "String: ", 8, 0);
            n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (n <= 0) break;
            buffer[n] = '\0';
            buffer[strcspn(buffer, "\n")] = '\0';
            int len = strlen(buffer);
            int pal = 1;
            for (int i = 0; i < len / 2; i++) {
                if (buffer[i] != buffer[len - 1 - i]) {
                    pal = 0;
                    break;
                }
            }
            if (pal == 1) {
                send(client_fd, "Palindrome\n", 11, 0);
            } else {
                send(client_fd, "Not Palindrome\n", 15, 0);
            }
        }
        else if (strcmp(buffer, "3") == 0) {
            send(client_fd, "Bye\n", 4, 0);
            break;
        }
        else {
            send(client_fd, "Invalid\n", 8, 0);
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
