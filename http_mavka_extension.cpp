struct MavkaHTTPExtHeader;
struct MavkaHTTPExtRequestData;

typedef void (*MavkaHTTPExtResponseSender)(void* request_data,
                                           int status,
                                           MavkaHTTPExtHeader* headers,
                                           int headers_size,
                                           char* body,
                                           int body_size);

typedef void (*MavkaHTTPExtRequestHandler)(void* request_data,
                                           char* method,
                                           char* path,
                                           MavkaHTTPExtHeader* headers,
                                           int headers_size,
                                           char* body,
                                           int body_size,
                                           MavkaHTTPExtResponseSender respond,
                                           void* userdata);
struct MavkaHTTPExtHeader {
  char* key;
  char* value;
};

struct MavkaHTTPExtRequestData {
  void* res;
  char* method;
  char* path;
  MavkaHTTPExtHeader* headers;
  int headers_size;
  char* body;
  int body_size;
  MavkaHTTPExtRequestHandler handler;
  void* handler_data;
};

#include "App.h"

extern "C" void start_http_server(int port,
                                  MavkaHTTPExtRequestHandler handler,
                                  void* data) {
  uWS::App()
      .any("/*",
           [handler, data](auto* res, auto* req) {
             char* method = new char[req->getMethod().size() + 1];
             memcpy(method, req->getMethod().data(), req->getMethod().size());
             method[req->getMethod().size()] = '\0';
             char* path = new char[req->getFullUrl().size() + 1];
             memcpy(path, req->getFullUrl().data(), req->getFullUrl().size());
             path[req->getFullUrl().size()] = '\0';
             MavkaHTTPExtHeader* headers =
                 new MavkaHTTPExtHeader[req->getHeaders().size()];
             int i = 0;
             for (auto& [key, value] : req->getHeaders()) {
               headers[i].key = new char[key.size() + 1];
               memcpy(headers[i].key, key.data(), key.size());
               headers[i].key[key.size()] = '\0';
               headers[i].value = new char[value.size() + 1];
               memcpy(headers[i].value, value.data(), value.size());
               headers[i].value[value.size()] = '\0';
               i++;
             }
             std::unique_ptr<std::string> body_buffer;
             res->onData([res, body_buffer = std::move(body_buffer), method,
                          path, headers, i, handler,
                          data](std::string_view chunk, bool isFin) mutable {
               if (isFin) {
                 if (body_buffer.get()) {
                   body_buffer->append(chunk);
                 } else {
                   body_buffer = std::make_unique<std::string>(chunk);
                 }
                 char* body = nullptr;
                 if (!body_buffer->empty()) {
                   body = new char[body_buffer->size()];
                   memcpy(body, body_buffer->c_str(), body_buffer->size());
                 }
                 auto* request_data = new MavkaHTTPExtRequestData(
                     res, method, path, headers, i, body, body_buffer->size(),
                     handler, data);
                 handler(
                     request_data, method, path, headers, i, body,
                     body_buffer->size(),
                     [](void* request_data, int status,
                        MavkaHTTPExtHeader* headers, int headers_size,
                        char* body, int body_size) {
                       MavkaHTTPExtRequestData* rd =
                           (MavkaHTTPExtRequestData*)request_data;
                       auto* res = (uWS::HttpResponse<false>*)rd->res;
                       for (int i = 0; i < headers_size; i++) {
                         res->writeHeader(headers[i].key, headers[i].value);
                       }
                       res->end(body, body_size);
                       if (rd->method) {
                         delete rd->method;
                       }
                       if (rd->path) {
                         delete rd->path;
                       }
                       if (rd->headers) {
                         for (int i = 0; i < rd->headers_size; i++) {
                           delete rd->headers[i].key;
                           delete rd->headers[i].value;
                         }
                         delete rd->headers;
                       }
                       if (rd->body) {
                         delete rd->body;
                       }
                       delete rd;
                     },
                     data);
               } else {
                 if (!body_buffer.get()) {
                   body_buffer = std::make_unique<std::string>(chunk);
                 } else {
                   body_buffer->append(chunk);
                 }
               }
             });
             res->onAborted([]() {});
           })
      .listen(port, [port](auto* token) {})
      .run();
}