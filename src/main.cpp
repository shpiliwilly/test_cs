#include <iostream>

#include <string>
#include "txt_protocol.h"
#include "order.h"
#include "engine.h"

class Notifier : public INotifier {
public:
    virtual void Notify(const std::string& trader, unsigned order_id) {
        std::cout << "Trader: " << trader << ", order " << order_id << " filled" << std::endl;
    }
};

int main() {

    Notifier notif;
    Engine engine(2, notif);

    std::cout << "starting the app" << std::endl;

    while (true) {
        // read the next order from the std in and handle it
        std::string currLine;
        std::getline(std::cin, currLine);
        Order* order = ParseOrder(currLine);
        if (!order) {
            std::cerr << "invalid input, line is ignored" << std::endl;
        } else {
            engine.HandleOrder(order);
        }

    }

}

