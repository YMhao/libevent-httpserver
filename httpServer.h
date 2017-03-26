#ifndef HTTPSERVER_H
#define HTTPSERVER_H

class HttpServer
{
public:
    HttpServer() {}
    ~HttpServer() {}
    bool servInit(const char *ip, int port, int nthreads);
protected:
    static void* dispatch(void *arg);
    static void genericHandler(struct evhttp_request *req, void *arg);
    void processRequest(struct evhttp_request *request);
    int bindSocket(const char *ip, int port);
};
#endif // HTTPSERVER_H
