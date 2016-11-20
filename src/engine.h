#pragma once

#include <set>
#include <map>
#include <list>
#include <string>

class Order;

struct INotifier {
    virtual void Notify(const std::string& trader, unsigned order_id) = 0;
};


class Engine {
public:

    Engine(unsigned num_threads, INotifier& notifier);
    void HandleOrder(Order* order);

    // storage of unmatched orders
    typedef std::list<Order*> OrderList;
    typedef std::map<std::string, OrderList> OrdersByStockName;
    typedef std::map<unsigned, OrdersByStockName> OrdersByThread;
    OrdersByThread m_orders;

    typedef std::set<unsigned, unsigned> OrderIdsByThread;

    OrderList& FindOrders(const std::string& stock_name);

private:
    INotifier& m_notifier;
};

