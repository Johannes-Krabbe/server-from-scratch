#include <malloc.h>
#include <string.h>
#include "parser.h"

const char* arr[] = { "OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT" };

static enum method parse_method(const char* str, size_t len) {
    for (int i = 0; i < 8; i++) {
        if (strncmp(str, arr[i], len) == 0) {
            return i;
        }
    }
    return -1;
}

int parse_request_line(struct request* request, const char* str, size_t len) {
    if (*str == ' ') {
        return -1;
    }
    size_t i = 1;

    // parse method
    while (1) {
        if (i == len || i == 10) {
            return -1;
        }

        if (str[i] == ' ') {
            enum method m = parse_method(str, i-1);
            if (m >= 0) {
                request->method = m;
                break;
            } else {
                return -1;
            }
        }
        i++;
    }

    do {
        i++;
    } while (str[i] == ' ');

    size_t start = i;
    while (1) {
        if (i == len) {
            return -1;
        }

        char c = str[i];
        if (c == '\r') {
            return -1;
        }
        if (c == ' ') {
            request->uri.str = str + start;
            request->uri.len = i - start;
            break;
        }
        i++;
    }

    // TODO check version
    return 0;
}
