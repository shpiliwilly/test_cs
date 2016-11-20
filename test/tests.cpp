#include "vtest.h"

#include <condition_variable>
#include <thread>
#include <mutex>
#include <chrono>
#include <stdexcept>

#include "../src/txt_protocol.h"
#include "../src/engine.h"

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

TEST_CASE(TxtProtocol, InputOk)
{
    std::string line("id:2341|trader:jack|stock:msft|quantity:15|side:0|");
    Order* order = ParseOrder(line);

    ASSERT(order);
    ASSERT(order->m_id == 2341);
    ASSERT(order->m_trader == "jack");
    ASSERT(order->m_stock == "msft");
    ASSERT(order->m_qnt == 15);
    ASSERT(order->m_side == BUY);
    SUCCEED();
}

TEST_CASE(TxtProtocol, InvalidInput1)
{
    Order* order = ParseOrder(std::string());
    ASSERT(!order);
    SUCCEED();
}

TEST_CASE(TxtProtocol, InvalidInput2)
{
    std::string line("id:2341|trer:jack|stock:msft|quantity:15|side:0|");
    Order* order = ParseOrder(line);
    ASSERT(!order);
    SUCCEED();
}

TEST_CASE(TxtProtocol, InvalidInput3)
{
    std::string line("id2341|trer:jack|stock:msft|quantity:15|side:0|");
    Order* order = ParseOrder(line);
    ASSERT(!order);
    SUCCEED();
}

TEST_CASE(TxtProtocol, InvalidInput4)
{
    std::string line("id:2341|trer:jack|stock:msfasdaasdfdasdfasdfat|quantity:15|side:0|");
    Order* order = ParseOrder(line);
    ASSERT(!order);
    SUCCEED();
}

TEST_CASE(TxtProtocol, InvalidInput5)
{
    std::string line("id:2341trer:jack|stock:msft|quantity:15|side:0|");
    Order* order = ParseOrder(line);
    ASSERT(!order);
    SUCCEED();
}

TEST_CASE(TxtProtocol, InvalidInput6)
{
    // str, but int expected
    std::string line("id:str|trader:jack|stock:msft|quantity:15|side:0|");
    Order* order = ParseOrder(line);
    ASSERT(!order);
    SUCCEED();
}

TEST_CASE(TxtProtocol, InvalidInput7)
{
    // invalid side value
    std::string line("id:2341|trader:jack|stock:msft|quantity:15|side:4|");
    Order* order = ParseOrder(line);
    ASSERT(!order);
    SUCCEED();
}

TEST_CASE(TxtProtocol, InvalidInput8)
{
    // incomplete
    std::string line("id:2341|trader:jack|stock:msft|quantity:15|");
    Order* order = ParseOrder(line);
    ASSERT(!order);
    SUCCEED();
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

typedef std::pair<std::string, unsigned> CallRecord;

struct NotifierMock : public INotifier {
    std::mutex m_mtx;
    std::condition_variable m_cv;

    // number of notification calls expected in a scenario
    unsigned m_calls_expected;
    // number of actul calls
    unsigned m_calls_count;

    // calls log
    std::list<CallRecord> m_calls;

    NotifierMock(unsigned calls_expected)
        : m_calls_expected(calls_expected)
        , m_calls_count(0)
    { }

    virtual void Notify(const std::string& trader, unsigned order_id) {
        std::unique_lock<std::mutex> lck(m_mtx);
        m_calls.push_back(CallRecord(trader, order_id));
        m_calls_count++;
        if(m_calls_count >= m_calls_expected) {
            m_cv.notify_one();
        }
    }

    // returns false in case of timeout, true otherwhise
    bool WaitForCalls(unsigned milli_sec = 1000) {
        std::unique_lock<std::mutex> lck(m_mtx);

        if(m_cv.wait_for(lck, std::chrono::milliseconds(milli_sec), [this](){ return m_calls_count >= m_calls_expected; } )) {
            return true;
        } else {
            // timeout
            return false;
        }
    }

    CallRecord GetNextCallEntry() {
        if(m_calls.empty()) 
            throw std::runtime_error("call log is empty");
        CallRecord ret = m_calls.front();
        m_calls.pop_front();
        return ret;
    }
};

///////////////////////////////////////////////////////////////////////////

TEST_CASE(Matching, Basic)
{
    unsigned expected_notifications_count = 2;

    // TODO. ignored for now
    unsigned num_threads = 2;
    NotifierMock notifier(expected_notifications_count);
    Engine engine(num_threads, notifier);

    // basic scenario:
    // Trader A places a buy order of 200 on stock S. 
    // Trader B places a sell order of 200 on stock S. 
    // Both notified

    Order* order_1 = ParseOrder("id:1|trader:jack|stock:msft|quantity:15|side:0|");
    Order* order_2 = ParseOrder("id:2|trader:bobo|stock:msft|quantity:15|side:1|");
    ASSERT(order_1);
    ASSERT(order_2);
    engine.HandleOrder(order_1);
    engine.HandleOrder(order_2);

    ASSERT(notifier.WaitForCalls());
    CallRecord call_1 = notifier.GetNextCallEntry();
    CallRecord call_2 = notifier.GetNextCallEntry();
    ASSERT(call_1.first == "jack" && call_1.second == 1);
    ASSERT(call_2.first == "bobo" && call_2.second == 2);

    SUCCEED();
}

///////////////////////////////////////////////////////////////////////////

TEST_CASE(Matching, NoMatch)
{
    unsigned expected_notifications_count = 2;

    // TODO. ignored for now
    unsigned num_threads = 2;
    NotifierMock notifier(expected_notifications_count);
    Engine engine(num_threads, notifier);

    // no match, different stocks
    // Trader A puts buy order on stock S
    // Trader B puts sell order on stock Q
    // no notifications

    Order* order_1 = ParseOrder("id:1|trader:jack|stock:msft|quantity:15|side:0|");
    Order* order_2 = ParseOrder("id:2|trader:bobo|stock:aasd|quantity:15|side:1|");
    ASSERT(order_1);
    ASSERT(order_2);
    engine.HandleOrder(order_1);
    engine.HandleOrder(order_2);

    ASSERT(!notifier.WaitForCalls());
    SUCCEED();
}

///////////////////////////////////////////////////////////////////////////

TEST_CASE(Matching, ManyPartialFills)
{
    unsigned expected_notifications_count = 4;

    // TODO. ignored for now
    unsigned num_threads = 2;
    NotifierMock notifier(expected_notifications_count);
    Engine engine(num_threads, notifier);

    // Trader C places a sell order of 300 on stock G. 
    // Trader D places a buy order of 200 on stock G. 
    // Notify the Trader D with success message. 
    // Trader E places a Buy Order of 200 on stock G. 
    // Notify the Trader C with success message.
    // Trader F places a Sell Order of 100 on stock Z. 
    // Trader Q places a Sell Order of 100 on stock G. 
    // Notify the Trader E with success message.
    // Notify the Trader Q with success message.

    Order* order_1 = ParseOrder("id:1|trader:C|stock:msft|quantity:300|side:1|");
    Order* order_2 = ParseOrder("id:2|trader:D|stock:msft|quantity:200|side:0|");
    Order* order_3 = ParseOrder("id:3|trader:E|stock:msft|quantity:200|side:0|");
    Order* order_4 = ParseOrder("id:4|trader:F|stock:zzzz|quantity:100|side:1|");
    Order* order_5 = ParseOrder("id:5|trader:Q|stock:msft|quantity:100|side:1|");
    ASSERT(order_1 && order_2 && order_3 && order_4 && order_5);
    engine.HandleOrder(order_1);
    engine.HandleOrder(order_2);
    engine.HandleOrder(order_3);
    engine.HandleOrder(order_4);
    engine.HandleOrder(order_5);

    ASSERT(notifier.WaitForCalls());
    CallRecord call_1 = notifier.GetNextCallEntry();
    CallRecord call_2 = notifier.GetNextCallEntry();
    CallRecord call_3 = notifier.GetNextCallEntry();
    CallRecord call_4 = notifier.GetNextCallEntry();
    ASSERT(call_1.first == "D" && call_1.second == 2);
    ASSERT(call_2.first == "C" && call_2.second == 1);
    ASSERT(call_3.first == "E" && call_3.second == 3);
    ASSERT(call_4.first == "Q" && call_4.second == 5);

    SUCCEED();
}

///////////////////////////////////////////////////////////////////////////

TEST_CASE(Matching, MultiMatch)
{
    unsigned expected_notifications_count = 4;

    // TODO. ignored for now
    unsigned num_threads = 2;
    NotifierMock notifier(expected_notifications_count);
    Engine engine(num_threads, notifier);

    // Trader W, X and Y place sell order of 200 on stock H each. 
    // Trade Z place a buy order of 600 on stock H. 
    // Trader W, X, Y and Z should be notified of success.

    Order* order_1 = ParseOrder("id:1|trader:W|stock:msft|quantity:200|side:1|");
    Order* order_2 = ParseOrder("id:2|trader:X|stock:msft|quantity:200|side:1|");
    Order* order_3 = ParseOrder("id:3|trader:Y|stock:msft|quantity:200|side:1|");
    Order* order_4 = ParseOrder("id:4|trader:Z|stock:msft|quantity:600|side:0|");
    ASSERT(order_1 && order_2 && order_3 && order_4);
    engine.HandleOrder(order_1);
    engine.HandleOrder(order_2);
    engine.HandleOrder(order_3);
    engine.HandleOrder(order_4);

    ASSERT(notifier.WaitForCalls());
    CallRecord call_1 = notifier.GetNextCallEntry();
    CallRecord call_2 = notifier.GetNextCallEntry();
    CallRecord call_3 = notifier.GetNextCallEntry();
    CallRecord call_4 = notifier.GetNextCallEntry();
    ASSERT(call_1.first == "W" && call_1.second == 1);
    ASSERT(call_2.first == "X" && call_2.second == 2);
    ASSERT(call_3.first == "Y" && call_3.second == 3);
    ASSERT(call_4.first == "Z" && call_4.second == 4);

    SUCCEED();
}

///////////////////////////////////////////////////////////////////////////

int main() {
    vtest::RunAllTests();
    return 0;
}


