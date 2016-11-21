#pragma once

#include <set>
#include <map>
#include <list>
#include <string>

#include "active.h"

class Order;
class INotifier;

class Engine {
    std::atomic_bool m_stopped;
    boost::lockfree::queue<Order*> m_queue;
    std::thread m_thr;

public:

    Engine(unsigned num_threads, INotifier& notifier);
    ~Engine();
    void PostOrder(Order* order);

private:

    static void ThreadFuncS(void* this_ptr);

    // storage of unmatched orders
    typedef std::list<Order*> OrderList;
    typedef std::map<std::string, OrderList> OrdersByStockName;
    OrdersByStockName m_orders;

    typedef std::set<unsigned, unsigned> OrderIdsByThread;

    INotifier& m_notifier;

    void HandleOrder(Order* order);


    struct OrderMsg : public Active::MsgBase { 
        Engine* m_daddy;
        Order* m_order;

        OrderMsg(Engine* daddy, Order* order);
        virtual ~OrderMsg();
        virtual void Execute();
    };

};

