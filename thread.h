#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>

class ThreadNotify
{
public:
    ThreadNotify();
    ~ThreadNotify();
	void Lock() { pthread_mutex_lock(&m_mutex); }
	void Unlock() { pthread_mutex_unlock(&m_mutex); }
	void Wait() { pthread_cond_wait(&m_cond, &m_mutex); }
	void Signal() { pthread_cond_signal(&m_cond); }
private:
	pthread_mutex_t 	m_mutex;
	pthread_mutexattr_t	m_mutexattr;
    
	pthread_cond_t 		m_cond;
};

#endif
