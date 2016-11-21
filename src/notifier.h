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

    static void ThreadFunc(void* this_ptr);

public:

    Notifier();
    ~Notifier();

    virtual void Notify(const std::string& trader, unsigned order_id);
};

