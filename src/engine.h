#pragma once

#include <map>
#include <list>
#include <string>

#include "notifier.h"
#include "active.h"

class Order;
class INotifier;


class Engine : public Active {
public:

    Engine(INotifier& notifier) 
        : m_notifier(notifier) 
    { }

    ~Engine() {
        Stop();
    }

    void PostOrder(Order* order) {
        PushItem(order);
    }

private:

    // storage of unmatched orders
    typedef std::list<Order*> OrderList;
    typedef std::map<std::string, OrderList> OrdersByStockName;
    OrdersByStockName m_orders;

    INotifier& m_notifier;

    virtual void HandleItem(void* item);
};

