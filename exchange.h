#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "OrderBook.h"

class Exchange {
    struct OrderMeta {
        Side   side;
        string symbol;
    };

    unordered_map<string, OrderBook> books_;
    unordered_map<uint64_t, OrderMeta> meta_;
    uint64_t next_id_ = 0;

    static string fmt_id(uint64_t id);
    Order make(Side side, OrderType type, double price, uint64_t qty);
    Order do_submit(const string& sym, Side side, OrderType type, double price, uint64_t qty);

public:
    Order limit_buy(const string& sym, double price, uint64_t qty);
    Order limit_sell(const string& sym, double price, uint64_t qty);
    Order market_buy(const string& sym, uint64_t qty);
    Order market_sell(const string& sym, uint64_t qty);
    bool cancel(uint64_t id);
    pair<bool, Order> modify(uint64_t id, double new_price, uint64_t new_qty);
    
    void show_book(const string& sym);
    void show_trades(const string& sym);
    void show_stats(const string& sym);
};

#endif 