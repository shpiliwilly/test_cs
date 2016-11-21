#include "notifier.h"

Notifier::Notifier() 
    : m_thr(std::thread(ThreadFunc, this))
    , m_queue(128)
    , m_stopped(false)
{ }

Notifier::~Notifier() {
    m_queue.push(nullptr);
    m_thr.join();
}

void Notifier::Notify(const std::string& trader, unsigned order_id) {
     while (!m_queue.push(new NotifyEntry(trader, order_id)))
         ;
}

void Notifier::ThreadFunc(void* this_ptr) {
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

