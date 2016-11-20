#include "engine.h"
#include "order.h"


unsigned GetThreadId(const Order* order) {
    // TODO
    return 0;
}

Engine::Engine(unsigned num_threads, INotifier& notifier) 
    : m_notifier(notifier) 
{ }

Engine::OrderList& Engine::FindOrders(const std::string& stock_name) {
    return m_orders[0][stock_name];
}

void Engine::HandleOrder(Order* order) {
    if (!order) return;
    //unsigned thr_id = GetThreadId(order->m_stock);

    OrderList& orders = FindOrders(order->m_stock);
    if (orders.empty()) {
        orders.push_back(order);
        return;
    }

    OrderList::iterator it = orders.begin();
    while (order->m_qnt 
            && it != orders.end() 
            && order->m_side != (*it)->m_side) 
    {
        // fill
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

