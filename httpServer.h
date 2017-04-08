#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "threadPool.h"

class HttpServer
{
public:
    HttpServer() {}
    ~HttpServer() {}
    bool servInit(const char *ip, int port, int nthreads, int nworkthreads);
protected:
    static void* dispatch(void *arg);
    static void genericHandler(struct evhttp_request *req, void *arg);
    void processRequest(struct evhttp_request *request);
    int bindSocket(const char *ip, int port);

    ThreadPool m_thread_pool;

};
#endif // HTTPSERVER_H
