#pragma once

#include <boost/lockfree/queue.hpp>
#include <atomic>
#include <thread>
#include <string>

struct INotifier {
    virtual void Notify(const std::string& trader, unsigned order_id) = 0;
};


class Notifier : public INotifier {
    std::atomic_bool m_stopped;
    typedef std::pair<std::string, unsigned> NotifyEntry;
    boost::lockfree::queue<NotifyEntry*> m_queue;
    std::thread m_thr;

public:

    Notifier() 
        : m_thr(std::thread(ThreadFuncS, this))
        , m_queue(128)
        , m_stopped(false)
    { }

    ~Notifier() {
        m_queue.push(nullptr);
        m_thr.join();
    }

    virtual void Notify(const std::string& trader, unsigned order_id) {
         while (!m_queue.push(new NotifyEntry(trader, order_id)))
             ;
    }

    static void ThreadFuncS(void* this_ptr) {
        Notifier* notifier = static_cast<Notifier*>(this_ptr);
        while(!notifier->m_stopped) {
            NotifyEntry* p = nullptr;
            if(notifier->m_queue.pop(p)) {
                if(p) {
                    std::cout << "Trader: " << p->first << ", order " << p->second << " filled" << std::endl;
                    delete p;
                } else {
                    notifier->m_stopped = true;
                }
            } 
        }
    }


};


