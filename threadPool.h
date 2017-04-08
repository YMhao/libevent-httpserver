#ifndef THREADPOOL_H_
#define THREADPOOL_H_
#include "thread.h"
#include "task.h"
#include <pthread.h>
#include <list>
#include <stdint.h>

using namespace std;

class ThreadPool;

class WorkerThread {
public:
    WorkerThread();
    ~WorkerThread();

    static void* StartRoutine(void* arg);

    void Start();
    void Execute();
    void PushTask(Task* pTask);

    void set_thread_idx(uint32_t idx) { m_thread_idx = idx; }
    uint32_t task_cnt() const;
    void set_parent_thread_pool(ThreadPool *parent_thread_pool);

private:

    uint32_t		m_thread_idx;
    uint32_t		m_task_cnt;
    pthread_t		m_thread_id;
    ThreadNotify	m_thread_notify;
    list<Task*>     m_task_list;
    ThreadPool      *m_parent_thread_pool;
};

class ThreadPool {
public:
    ThreadPool();
    virtual ~ThreadPool();

    int Init(uint32_t worker_size);
    void AddTask(Task* pTask);
    void Destory();
    void setidle_thread_idx(const pthread_t &idle_thread_idx);

    uint32_t busy_thread_idx() const;
    void update_busy_thread_idx();
    void clean_busy_thread_idx();

private:
    uint32_t    m_idle_thread_idx;  // idle thread id 空闲的线程编号
    uint32_t    m_worker_size;
    WorkerThread* 	m_worker_list;

    uint32_t m_busy_thread_idx;   // 在线程池中，某个线程做完任务后，在该空闲线程中更新这个值，
                                  // 然后m_busy_thread_idx线程允许任务再分配，并将m_busy_thread_idx赋值为m_worker_size
};


#endif /* THREADPOOL_H_ */
