#include "threadPool.h"
#include <stdlib.h>
#include <sys/sysinfo.h>
#include "log.h"


WorkerThread::WorkerThread()
{
	m_task_cnt = 0;
    m_parent_thread_pool = NULL;
}

WorkerThread::~WorkerThread()
{

}

void* WorkerThread::StartRoutine(void* arg)
{
    WorkerThread* pThread = (WorkerThread*)arg;

	pThread->Execute();

	return NULL;
}

void WorkerThread::Start()
{
	(void)pthread_create(&m_thread_id, NULL, StartRoutine, this);
}

void WorkerThread::Execute()
{
	while (true) {
		m_thread_notify.Lock();

		// put wait in while cause there can be spurious wake up (due to signal/ENITR)
		while (m_task_list.empty()) {
            m_parent_thread_pool->setidle_thread_idx(m_thread_idx);
            m_parent_thread_pool->update_busy_thread_idx();
			m_thread_notify.Wait();
		}

        Task* pTask = m_task_list.front();
		m_task_list.pop_front();
        --m_task_cnt;
		m_thread_notify.Unlock();

        pTask->run();

        // 资源再分配
        if(m_thread_idx == m_parent_thread_pool->busy_thread_idx() && m_task_cnt > 10 ) {
            uint32_t pop_task_cnt = m_task_cnt >> 1; // 分配一半出去
            Task* pTask = NULL;
            for(int i=0; i< pop_task_cnt; i++) {
                m_thread_notify.Lock();
                pTask = m_task_list.front();
                m_task_list.pop_front();
                --m_task_cnt;
                m_thread_notify.Unlock();
                m_parent_thread_pool->AddTask(pTask);
            }
            m_parent_thread_pool->clean_busy_thread_idx();
        }

		delete pTask;
	}
}

void WorkerThread::PushTask(Task* pTask)
{
	m_thread_notify.Lock();
	m_task_list.push_back(pTask);
    ++m_task_cnt;
	m_thread_notify.Signal();
	m_thread_notify.Unlock();
}

uint32_t WorkerThread::task_cnt() const
{
    return m_task_cnt;
}

void WorkerThread::set_parent_thread_pool(ThreadPool *parent_thread_pool)
{
    m_parent_thread_pool = parent_thread_pool;
}

ThreadPool::ThreadPool()
{
    m_worker_size = 0;
    m_worker_list = NULL;
    m_busy_thread_idx = 0;
}

ThreadPool::~ThreadPool()
{

}


int ThreadPool::Init(uint32_t worker_size)
{
    if(worker_size == 0) {
        worker_size = get_nprocs();
        worker_size *=2;
        LOG_DEBUG("worker_size %d",worker_size);
    }

    m_worker_size = worker_size;
    m_worker_list = new WorkerThread [m_worker_size];
	if (!m_worker_list) {
		return 1;
	}

	for (uint32_t i = 0; i < m_worker_size; i++) {
        m_worker_list[i].set_thread_idx(i);
        m_worker_list[i].set_parent_thread_pool(this);
		m_worker_list[i].Start();
	}

    m_idle_thread_idx = 0;
    m_busy_thread_idx = m_worker_size;
	return 0;
}

void ThreadPool::Destory()
{
    if(m_worker_list)
        delete [] m_worker_list;
}

void ThreadPool::setidle_thread_idx(const pthread_t &idle_thread_idx)
{
    m_idle_thread_idx = idle_thread_idx;
}

uint32_t ThreadPool::busy_thread_idx() const
{
    return m_busy_thread_idx;
}

void ThreadPool::update_busy_thread_idx()
{
    uint32_t _busy_thread_idx=m_worker_size;
    uint32_t max_task_cnt=0;
    uint32_t tmp_cnt;
    for(uint32_t i=0; i<m_worker_size; i++) {
        tmp_cnt = m_worker_list[i].task_cnt();
        if(max_task_cnt < tmp_cnt) {
            max_task_cnt = tmp_cnt;
            _busy_thread_idx = i;
        }
    }
    m_busy_thread_idx = _busy_thread_idx;
}

void ThreadPool::clean_busy_thread_idx()
{
    m_busy_thread_idx = m_worker_size;
}

void ThreadPool::AddTask(Task* pTask)
{
    /*
     * select a random thread to push task
     * we can also select a thread that has less task to do
	 * but that will scan the whole thread list and use thread lock to get each task size
	 */
    //uint32_t thread_idx = random() % m_worker_size;
    //m_worker_list[thread_idx].PushTask(pTask);

    uint32_t idle_thread_idx = m_idle_thread_idx;
    uint32_t task_cnt = m_worker_list[m_idle_thread_idx].task_cnt();
    if(task_cnt > 1) {
        uint32_t tmp_cnt=0;
        // select a idle thread 选择一个空闲的线程
        for(uint32_t i=0; i<m_worker_size; i++) {
            tmp_cnt = m_worker_list[i].task_cnt();
            if(tmp_cnt == 0) {
                idle_thread_idx = i;
                break;
            }
            else {
                if(tmp_cnt < task_cnt) {
                    idle_thread_idx = i;
                    task_cnt = tmp_cnt;
                }
            }
        }
    }

    m_worker_list[idle_thread_idx].PushTask(pTask);
}

