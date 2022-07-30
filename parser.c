#include <malloc.h>
#include "parser.h"

struct request* parse_request(char* str, size_t len) {
    struct request* request = malloc(sizeof (struct request));
    return request;
}
