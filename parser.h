#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

enum method { INVALID = -1, OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT };
enum request_header { INVALID_ = -1, ACCEPT, ACCEPT_CHARSET, ACCEPT_ENCODING, ACCEPT_LANGUAGE, AUTHORIZATION, EXPECT, FROM, HOST, IF_MATCH, IF_MODIFIED_SINCE, IF_NONE_MATCH, IF_RANGE, IF_UNMODIFIED_SINCE, MAX_FORWARDS, PROXY_AUTHORIZATION, RANGE, REFERER, TE, USER_AGENT, CACHE_CONTROL, CONNECTION, DATE, PRAGMA, TRAILER, TRANSFER_ENCODING, UPGRADE, VIA, WARNING, ALLOW, CONTENT_ENCODING, CONTENT_LANGUAGE, CONTENT_LENGTH, CONTENT_LOCATION, CONTENT_MD5, CONTENT_RANGE, CONTENT_TYPE, EXPIRES, LAST_MODIFIED };


extern const size_t METHOD_COUNT;
extern const char* method_arr[];
extern const size_t HEADER_COUNT;
extern const char* header_arr[];

typedef struct {
    size_t len;
    const char* str;
} string;

struct request {
    enum method method;
    string uri;
    string* headers;
    string body;
};

struct request* parse_request(const char* str, size_t len);

int parse_request_line(struct request* request, const char* str, size_t len);

int parse_header(struct request* request, const char* str, size_t len);
#endif
