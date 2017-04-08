#include "httpTask.h"
#include <event.h>
#include <evhttp.h>
#include <event2/thread.h>
#include <pthread.h>
#include <unistd.h>

#include "log.h"

HttpTask::HttpTask(struct evhttp_request *req)
    : m_req(req)
{

}

HttpTask::~HttpTask()
{

}

void HttpTask::run()
{
    struct evbuffer *buf_input = evhttp_request_get_input_buffer(m_req);
    if(!buf_input) {
        LOG_ERROR("evhttp_request_get_input_buffer failed\n");
        return;
    }
    // 如果要跨线程，evbuffer要加锁
    // Enable locking on an evbuffer so that it can safely be used by multiple threads at the same time.
    evbuffer_enable_locking(buf_input,NULL);

    struct evbuffer *buf = evhttp_request_get_output_buffer(m_req);
    if(!buf) {
        LOG_ERROR("evhttp_request_get_output_buffer failed\n");
        return;
    }

    // 如果要跨线程，evbuffer要加锁
    // Enable locking on an evbuffer so that it can safely be used by multiple threads at the same time.
    evbuffer_enable_locking(buf,NULL);
    evbuffer_lock(buf);
    evbuffer_add_printf(buf, "thread_idx:%ld uri: %s\n",pthread_self(), evhttp_request_uri(m_req));
    evhttp_send_reply(m_req, HTTP_OK, "OK", buf);
    evbuffer_unlock(buf);

}

HttpReqTask::HttpReqTask(evhttp_request *req)
{

}

HttpReqTask::~HttpReqTask()
{

}

void HttpReqTask::run()
{

}
