#include <string.h>
#include "parser.h"


const size_t METHOD_COUNT = 8;
const char* method_arr[] = {"OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT" };
const size_t HEADER_COUNT = 38;
const char* header_arr[] = {"accept", "accept-charset", "accept-encoding", "accept-language", "authorization", "expect", "from", "host", "if-match", "if-modified-since", "if-none-match", "if-range", "if-unmodified-since", "max-forwards", "proxy-authorization", "range", "referer", "te", "user-agent", "cache-control", "connection", "date", "pragma", "trailer", "transfer-encoding", "upgrade", "via", "warning", "allow", "content-encoding", "content-language", "content-length", "content-location", "content-md5", "content-range", "content-type", "expires", "last-modified" };

static enum method parse_method(const char* str, size_t len) {
    for (int i = 0; i < 8; i++) {
        if (strncmp(str, method_arr[i], len) == 0) {
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


static enum request_header parse_header_name(const char* str, size_t len) {
    for (int i = 0; i < 38; i++) {
        if (strncmp(str, header_arr[i], len) == 0) {
            return i;
        }
    }
    return -1;
}

int parse_header(struct request* request, const char* str, size_t len) {
    size_t colon = 0;

    for (unsigned i = 0; i < len; i++) {
        if (str[i] == ':') {
            colon = i;
            break;
        }
    }

    if (colon == 0) {
        return -1;
    }

    char name[colon];

    for (unsigned i = 0; i < colon; i++) {
        char c = str[i];
        if (c == '-') {
            name[i] = c;
        } else {
            name[i] = (char) (c | 0x20);
        }
    }

    enum request_header h = parse_header_name(name, colon);

    if (h >= 0) {
        (request->headers)[h].str = str + colon + 2;
        (request->headers)[h].len = len - colon - 2;
    } else {
        return -1;
    }

    return 0;
}
