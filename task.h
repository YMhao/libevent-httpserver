#ifndef __TASK_H__
#define __TASK_H__

class Task {
public:
    Task(){}
    virtual ~Task(){}
    
    virtual void run() = 0;
private:
};

#endif /*defined(__TASK_H__) */
