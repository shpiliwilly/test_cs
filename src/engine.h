#pragma once

#include "notifier.h"
#include <map>
#include <list>
#include <string>

class Order;
class INotifier;


class Engine {
    std::atomic_bool m_stopped;
    boost::lockfree::queue<Order*> m_queue;
    std::thread m_thr;

public:

    Engine(INotifier& notifier);
    ~Engine();

    void PostOrder(Order* order);

private:

    // storage of unmatched orders
    typedef std::list<Order*> OrderList;
    typedef std::map<std::string, OrderList> OrdersByStockName;
    OrdersByStockName m_orders;

    INotifier& m_notifier;

    static void ThreadFunc(void* this_ptr);
    void HandleOrder(Order* order);
};

