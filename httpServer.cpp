#include "httpServer.h"

#include <event.h>
#include <evhttp.h>
#include <event2/thread.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>

#include "log.h"
#include "httpTask.h"

using namespace std;

static bool _SetAddr(const char* ip, const uint16_t port, sockaddr_in* pAddr)
{
    memset(pAddr, 0, sizeof(sockaddr_in));
    pAddr->sin_family = AF_INET;
    pAddr->sin_port = htons(port);
    //pAddr->sin_addr.s_addr = INADDR_ANY;
    pAddr->sin_addr.s_addr = inet_addr(ip);
    if (pAddr->sin_addr.s_addr == INADDR_NONE) {
        hostent* host = gethostbyname(ip);
        if (host == NULL) {
            return false;
        }
        pAddr->sin_addr.s_addr = *(uint32_t*)host->h_addr;
    }
    return true;
}

int HttpServer::bindSocket(const char *ip,int port)
{
    int retr;
    int nfd;
    nfd = socket(AF_INET, SOCK_STREAM, 0);
    if (nfd < 0) return -1;

    int one = 1;
    retr = setsockopt(nfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));

    struct sockaddr_in addr;
    if(!_SetAddr(ip,port, &addr))
        return -1;

    retr = bind(nfd, (struct sockaddr*)&addr, sizeof(addr));
    if (retr < 0) return -1;
    retr = listen(nfd, 10240);
    if (retr < 0) return -1;

    int flags;
    if ((flags = fcntl(nfd, F_GETFL, 0)) < 0
            || fcntl(nfd, F_SETFL, flags | O_NONBLOCK) < 0)
        return -1;


    return nfd;
}

bool HttpServer::servInit(const char *ip,int port, int nthreads, int nworkthreads) {
    int ret;
    int sfd = bindSocket(ip,port);
    if (sfd < 0) {
        LOG_ERROR("Bind %s:%u failed.",ip,port);
        return false;
    }
    LOG_DEBUG("Bind %s:%u success.",ip,port);

    if(evthread_use_pthreads()!=0) {
           LOG_DEBUG("evthread_use_pthreads failed.");
    }

    // set thread_pool size 设置线程池的线程数
    m_thread_pool.Init(nworkthreads);

    pthread_t threads[nthreads];
    for (int i = 0; i < nthreads; i++) {
        struct event_base *base = event_init();
        if (base == NULL)
            return false;

        struct evhttp *httpd = evhttp_new(base);
        if (httpd == NULL)
            return false;

        ret = evhttp_accept_socket(httpd, sfd);
        if (ret != 0)
            return false;

        evhttp_set_gencb(httpd, HttpServer::genericHandler, this);
        ret = pthread_create(&threads[i], NULL, HttpServer::dispatch, base);
        if (ret != 0)
            return false;
    }
    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }
    return true;
}

void* HttpServer::dispatch(void *arg) {
    event_base_dispatch((struct event_base*)arg);
    return NULL;
}

void HttpServer::genericHandler(struct evhttp_request *req, void *arg) {
    ((HttpServer*)arg)->processRequest(req);
}

/* Response codes  define in libevent */
//#define HTTP_OK			200	/**< request completed ok */
//#define HTTP_NOCONTENT		204	/**< request does not have content */
//#define HTTP_MOVEPERM		301	/**< the uri moved permanently */
//#define HTTP_MOVETEMP		302	/**< the uri moved temporarily */
//#define HTTP_NOTMODIFIED	304	/**< page was not modified from last */
//#define HTTP_BADREQUEST		400	/**< invalid http request was made */
//#define HTTP_NOTFOUND		404	/**< could not find content for uri */
//#define HTTP_BADMETHOD		405 	/**< method not allowed for this uri */
//#define HTTP_ENTITYTOOLARGE	413	/**<  */
//#define HTTP_EXPECTATIONFAILED	417	/**< we can't handle this expectation */
//#define HTTP_INTERNAL           500     /**< internal error */
//#define HTTP_NOTIMPLEMENTED     501     /**< not implemented */
//#define HTTP_SERVUNAVAIL	503	/**< the server is not available */

void HttpServer::processRequest(struct evhttp_request *req) {
      HttpTask *task = new HttpTask(req);
      m_thread_pool.AddTask(task);

//    struct evbuffer *buf_input = evhttp_request_get_input_buffer(req);
//    if(!buf_input) {
//        LOG_ERROR("evhttp_request_get_input_buffer failed\n");
//        return;
//    }
//    // 如果要跨线程，evbuffer要加锁
//    // Enable locking on an evbuffer so that it can safely be used by multiple threads at the same time.
//    evbuffer_enable_locking(buf_input,NULL);

//    struct evbuffer *buf = evhttp_request_get_output_buffer(req);
//    if(!buf) {
//        LOG_ERROR("evhttp_request_get_output_buffer failed\n");
//        return;
//    }

//    // 如果要跨线程，evbuffer要加锁
//    // Enable locking on an evbuffer so that it can safely be used by multiple threads at the same time.
//    evbuffer_enable_locking(buf,NULL);
//    evbuffer_lock(buf);
//    evbuffer_add_printf(buf, "threadid:%ld uri: %s\n",pthread_self(), evhttp_request_uri(req));
//    evhttp_send_reply(req, HTTP_OK, "OK", buf);
//    evbuffer_unlock(buf);
}
