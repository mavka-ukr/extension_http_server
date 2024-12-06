#define HTTPSERVER_IMPL
#define EPOLL

#include "httpserver.h"

#define RESPONSE "Hello, World!"

typedef struct xxx_header {
    char *key;
    char *value;
} xxx_header;

typedef void (*xxx_respond)(void *request, int status, struct xxx_header *headers, int headers_size, char *body,
                            int body_size);

typedef void (*xxx_handler)(void *request, char *path, struct xxx_header *headers, int headers_size, char *body,
                            int body_size,
                            xxx_respond respond,
                            void *userdata);

typedef struct xxx_userdata {
    xxx_handler handler;
    void *data;
} xxx_userdata;

void xxx_responder(void *request, int status, struct xxx_header *headers, int headers_size, char *body,
                   int body_size) {
    struct http_response_s *response = http_response_init();
    http_response_status(response, status);
    for (int i = 0; i < headers_size; i++) {
        http_response_header(response, headers[i].key, headers[i].value);
    }
    http_response_body(response, body, body_size);
    http_respond(request, response);
}

void handle_request(struct http_request_s *request) {
    struct xxx_userdata *userdata = http_request_server_userdata(request);
    struct http_string_s target = http_request_target(request);
    char *path = malloc(target.len);
    memcpy(path, target.buf, target.len);
    userdata->handler(request, path, NULL, 0, NULL, 0, xxx_responder, userdata->data);
    free(path);
}

extern void start_http_server(int port, xxx_handler handler, void *data) {
    struct http_server_s *server = http_server_init(port, handle_request);
    struct xxx_userdata *userdata = malloc(sizeof(struct xxx_userdata));
    userdata->handler = handler;
    userdata->data = data;
    http_server_set_userdata(server, userdata);
    http_server_listen(server);
}