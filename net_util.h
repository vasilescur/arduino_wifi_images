#ifndef NET_UTIL
#define NET_UTIL
#endif

#ifndef FUTURES
#include "futures.h"
#endif

#include <WiFiNINA.h>

#define HTTP_GET 0
#define HTTP_POST 1
typedef int HttpRequestType;

typedef struct HttpGetJob {
    WiFiClient* client;
    bool done;
    char* response_buf;
    size_t response_buf_size;
    future* future;
} HttpGetJob;

typedef struct HttpGetJobListNode {
    HttpGetJob* job;
    struct HttpGetJobListNode* next;
} HttpGetJobListNode;

int net_setup();
void print_wifi_status();

future http_get(String hostname, int port, String path);
void http_get_wait_loop();
