#include "engine.h"
#include "order.h"


Engine::Engine(INotifier& notifier) 
    : m_notifier(notifier) 
    , m_thr(std::thread(ThreadFunc, this))
    , m_queue(128)
    , m_stopped(false)
{ }

Engine::~Engine() {
    m_queue.push(nullptr);
    m_thr.join();
}

void Engine::PostOrder(Order* order) {
     while (!m_queue.push(order))
         ;
}

void Engine::ThreadFunc(void* this_ptr) {
    Engine* engine = static_cast<Engine*>(this_ptr);
    while(!engine->m_stopped) {

        Order* p = nullptr;
        if(engine->m_queue.pop(p)) {
            if(p) {
                engine->HandleOrder(p);
            } else {
                engine->m_stopped = true;
            }
        } 

    }
}

void Engine::HandleOrder(Order* order) {
    if (!order) 
        return;

    OrderList& orders = m_orders[order->m_stock];
    if (orders.empty()) {
        orders.push_back(order);
        return;
    }

    OrderList::iterator it = orders.begin();
    while (order->m_qnt 
            && it != orders.end() 
            && order->m_side != (*it)->m_side) 
    {
        // match && fill
        unsigned fill_volume = std::min((*it)->m_qnt, order->m_qnt);
        (*it)->m_qnt -= fill_volume;
        order->m_qnt -= fill_volume;
        if (!(*it)->m_qnt) {
            m_notifier.Notify((*it)->m_trader, (*it)->m_id);
            delete *it;
            orders.erase(it++);
            continue;
        }
        it++;
    }

    if (order->m_qnt) {
        orders.push_back(order);
    } else {
        m_notifier.Notify(order->m_trader, order->m_id);
        delete order;
    }

}

