#include "engine.h"
#include "order.h"

void Engine::HandleItem(void* item) {
    Order* order = static_cast<Order*>(item);

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

