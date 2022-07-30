#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include "parser.h"

static void print_headers(string* headers) {
    printf("Headers:\n");
    for (unsigned i = 0; i < HEADER_COUNT; i++) {
        const char* str = headers[i].str;
        if (str != NULL) {
            printf("> %s: %.*s", header_arr[i], (int) headers[i].len, str);
        }
    }
}

int main() {
    printf("Hello, Jakob!\n");

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("Socket creation");
        return 1;
    }

    int opt = 1;

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        return(1);
    }

    struct in_addr in_addr;
    in_addr.s_addr = htonl(INADDR_LOOPBACK);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8082);
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

    struct request* request = calloc(1, sizeof (struct request));
    string* headers = calloc(38, sizeof (string));
    request->headers = headers;

    char* message = "Hello\n";

    char* beginning = buffer;
    while (1) {
        int req = accept(sock, (struct sockaddr*)&addr, &addrlen);
        if (req == -1) {
            perror("accept");
            return 1;
        }

        ssize_t res = 1;
        while (1) {
            res = read(req, buffer + bufflen, maxlen - bufflen);

            for (int i = 0; i < res; i++) {
                if (buffer[bufflen + i] == '\n') {
                    size_t len = buffer + bufflen + i - beginning - 1;
                    printf("LINE: %.*s\n", (int)len, beginning);

                    if (len == 0) {
                        goto loopend;
                    }

                    // analysis
                    if (beginning == buffer) {
                        if (parse_request_line(request, beginning, len) == -1) {
                            // TODO error handling
                        }
                    } else {
                        if (parse_header(request, beginning, len) == -1) {
                            printf("error parsing header");
                            // TODO error handling
                        }
                    }

                    beginning = buffer + bufflen + i + 1;
                }
            }

            if (res <= 0) {
                if (res == 0) {
                    // TODO
                    break;
                } else {
                    perror("read");
                    return 1;
                }
            }

            bufflen += res;

            if (bufflen == maxlen) {
                break;
            }
        }

        loopend:
        printf("END OF HEADERS\n");

        //printf("Received message: %s\n", buffer);

        //struct request* request = parse_request(buffer, bufflen);
        printf("METHOD: %i\n", request->method);
        printf("URI: %.*s\n", (int) request->uri.len, request->uri.str);
        print_headers(request->headers);

        if (send(req, message, strlen(message), 0) == -1) {
            perror("send");
            return 1;
        }

        if (close(req) == -1) {
            perror("close");
            return 1;
        }

        bufflen = 0;
    }
    return 0;
}
