#pragma once

#include <string>

enum Side {
    BUY = 0,
    SELL
};

struct Order {
    Order(unsigned id, const std::string& trader, const std::string& stock, unsigned qnt, Side side)
        : m_id(id)
        , m_trader(trader)
        , m_stock(stock)
        , m_qnt(qnt)
        , m_side(side)
    { }

    unsigned m_id;
    std::string m_trader;
    std::string m_stock;
    unsigned m_qnt;
    Side m_side;
};

