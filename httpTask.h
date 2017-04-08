#ifndef HTTPTASK_H
#define HTTPTASK_H

#include "task.h"
#include <evhttp.h>

class HttpTask : public Task
{
public:
    HttpTask(struct evhttp_request *req);
    ~HttpTask();
    void run();
private:
    struct evhttp_request *m_req;

};

class HttpReqTask: public Task
{
public:
    HttpReqTask(struct evhttp_request *req);
    ~HttpReqTask();
    void run();
private:
    struct evhttp_request *m_req;
};

#endif // HTTPTASK_H
