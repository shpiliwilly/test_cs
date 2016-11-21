#include <iostream>
#include <thread>
#include <string>

#include "txt_protocol.h"
#include "order.h"
#include "engine.h"
#include "notifier.h"

////////////////////////////////////////////////////////////////////////

int main() {

    unsigned num_cpu = std::thread::hardware_concurrency();

    Notifier notifier;
    Engine engine(num_cpu, notifier);

    std::cout << "Engine started." << std::endl;
    std::cout << "Input order in the following format: \"id:123|trader:jack|stock:msft|quantity:15|side:0|\"" << std::endl;
    std::cout << "Or press Ctrl+D to stop" << std::endl;

    std::string currLine;
    while (std::getline(std::cin, currLine)) {
        // read the next order from the std in and handle it
        Order* order = ParseOrder(currLine);
        if (!order) {
            std::cerr << "invalid input, line is ignored" << std::endl;
        } else {
            engine.PostOrder(order);
        }
    }

}

