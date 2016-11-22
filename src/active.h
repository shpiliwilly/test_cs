#pragma once

#include <boost/lockfree/queue.hpp>
#include <atomic>
#include <thread>

class Active {
    std::atomic_bool m_stopped;
    boost::lockfree::queue<void*> m_queue;
    std::thread m_thr;

    static void ThreadFunc(void* this_p) {
        Active* this_ptr = static_cast<Active*>(this_p);
        while(!this_ptr->m_stopped) {
            void* p = nullptr;
            if(this_ptr->m_queue.pop(p)) {
                if(p) {
                    this_ptr->HandleItem(p);
                } else {
                    this_ptr->m_stopped = true;
                }
            } 

        }
    }

protected:

    void PushItem(void* item) {
         while (!m_queue.push(item))
             ;
    }

    virtual void HandleItem(void* item) = 0;

public:

    Active() 
        : m_thr(std::thread(ThreadFunc, this))
        , m_queue(128)
        , m_stopped(false)
    { }

    virtual ~Active() { }

    void Stop() {
        m_queue.push(nullptr);
        m_thr.join();
    }

};

