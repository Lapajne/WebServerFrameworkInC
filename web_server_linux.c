#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);
    printf("Server listening on port 8080...\n");

    while (1) {
        int client = accept(server_fd, NULL, NULL);
        char response[] = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
        send(client, response, sizeof(response) - 1, 0);
        close(client);
    }

    close(server_fd);
    return 0;
}
