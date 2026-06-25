#ifndef ORDER_H
#define ORDER_H

#include <bits/stdc++.h>

using namespace std;

enum class Side { BUY, SELL };
enum class OrderType { LIMIT, MARKET };

struct Order {
    uint64_t  id         = 0;
    Side      side       = Side::BUY;
    OrderType type       = OrderType::LIMIT;
    double    price      = 0.0;
    uint64_t  quantity   = 0;
    uint64_t  timestamp  = 0;
    uint64_t  filled_qty = 0;

    Order() = default;
    Order(uint64_t id, Side s, OrderType t,
          double p, uint64_t q, uint64_t ts)
        : id(id), side(s), type(t),
          price(p), quantity(q), timestamp(ts) {}

    uint64_t remaining() const { return quantity - filled_qty; }
};

struct Trade {
    uint64_t trade_id;
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    double   price;
    uint64_t quantity;

    string str() const {
        ostringstream o;
        o << "TRADE #" << setw(4) << setfill('0') << trade_id
          << "  BUY #"  << setw(4) << buy_order_id
          << "  SELL #" << setw(4) << sell_order_id
          << "  at " << fixed << setprecision(2) << price
          << " x" << quantity;
        return o.str();
    }
};

#endif 