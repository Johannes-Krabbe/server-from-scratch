#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

int main() {
    printf("Hello, Jakob!\n");

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("Socket creation");
        return 1;
    }

    struct in_addr in_addr;
    in_addr.s_addr = htonl(INADDR_LOOPBACK);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr = in_addr;

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }

    printf("Successfully bound to 127.0.0.1:8080");

    if (listen(sock, 10)) {
        perror("listen");
        return 1;
    }

    socklen_t addrlen = sizeof(addr);

    char buffer[1024] = {0};
    char* message = "Hello\n";
    while (1) {
        int req = accept(sock, (struct sockaddr*)&addr, &addrlen);
        if (req == -1) {
            perror("accept");
            return 1;
        }

        read(req, buffer, 1024);
        printf("Received message: %s", buffer);

        if (send(req, message, strlen(message), 0) == -1) {
            perror("send");
            return 1;
        }

        if (close(req) == -1) {
            perror("close");
            return 1;
        }


    }
    return 0;
}
