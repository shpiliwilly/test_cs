#pragma once

#include <string>

#include "active.h"

struct INotifier {
    virtual ~INotifier() { }
    virtual void Notify(const std::string& trader, unsigned order_id) = 0;
};

class Notifier : public INotifier
               , public Active
{
    typedef std::pair<std::string, unsigned> NotifyItem;

    virtual void HandleItem(void* item) {
        NotifyItem* p = static_cast<NotifyItem*>(item);
        std::cout << "Trader: " << p->first << ", order " << p->second << " filled" << std::endl;
        delete p;
    }

public:

    ~Notifier() {
        Stop();
    }

    virtual void Notify(const std::string& trader, unsigned order_id) {
        PushItem(new NotifyItem(trader, order_id));
    }

};
