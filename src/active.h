#pragma once

#include <boost/lockfree/queue.hpp>
#include <atomic>
#include <thread>
#include <chrono>

class Active {
    std::atomic_bool m_stopped;
    boost::lockfree::queue<void*> m_queue;
    std::thread m_thr;

    static void ThreadFunc(void* this_p) {
        unsigned empty_spin_count = 0;
        const unsigned max_empty_spin_count = 200;

        Active* this_ptr = static_cast<Active*>(this_p);
        while(!this_ptr->m_stopped) {
            void* p = nullptr;
            if(this_ptr->m_queue.pop(p)) {
                empty_spin_count = 0;
                if(p) {
                    this_ptr->HandleItem(p);
                } else {
                    this_ptr->m_stopped = true;
                }
            } else {
                // queue has been empty for long enough, stop heating cpu
                ++empty_spin_count;
                if(empty_spin_count > max_empty_spin_count) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

