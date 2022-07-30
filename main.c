#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

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

    printf("Successfully bound to 127.0.0.1:8080\n");

    if (listen(sock, 10)) {
        perror("listen");
        return 1;
    }

    socklen_t addrlen = sizeof(addr);

    char* buffer = malloc(10 * 1024 * 1024);
    if (buffer == NULL) {
        fprintf(stderr, "Unable to allocate buffer\n");
        return 1;
    }
    size_t bufflen = 0;
    size_t maxlen = 10 * 1024 * 1024;

    char* message = "Hello\n";


    while (1) {
        int req = accept(sock, (struct sockaddr*)&addr, &addrlen);
        if (req == -1) {
            perror("accept");
            return 1;
        }

        ssize_t res = 1;
        while (1) {
            res = read(req, buffer + bufflen, maxlen - bufflen);
            bufflen += res;

            if (res <= 0) {
                if (res == 0) {
                    break;
                } else {
                    perror("read");
                    return 1;
                }
            }
            if (bufflen == maxlen) {
                break;
            }
        }

        printf("Received message: %s\n", buffer);

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
