#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"

struct BidComp {
    bool operator()(const Order& a, const Order& b) const {
        if (a.price != b.price) return a.price < b.price;
        return a.timestamp > b.timestamp;
    }
};

struct AskComp {
    bool operator()(const Order& a, const Order& b) const {
        if (a.price != b.price) return a.price > b.price;
        return a.timestamp > b.timestamp;
    }
};

class OrderBook {
    using BidPQ = priority_queue<Order, vector<Order>, BidComp>;
    using AskPQ = priority_queue<Order, vector<Order>, AskComp>;

    BidPQ bids_;
    AskPQ asks_;

    unordered_set<uint64_t> resting_;     
    unordered_set<uint64_t> cancelled_;   

    vector<Trade> trades_;
    uint64_t      trade_seq_ = 0;

    void clean_bids();
    void clean_asks();
    void emit_trade(const Order& buyer, const Order& seller, double price, uint64_t qty);
    void match_buy(Order& buyer);
    void match_sell(Order& seller);

    template<typename PQ>
    vector<pair<double, uint64_t>> aggregate(PQ& pq) {
        vector<pair<double, uint64_t>> levels;
        vector<Order> valid;
        while (!pq.empty()) {
            Order o = pq.top(); pq.pop();
            if (cancelled_.count(o.id)) {
                cancelled_.erase(o.id);       
                continue;
            }
            valid.push_back(o);
            auto it = find_if(levels.begin(), levels.end(),
                [&](auto& p) { return p.first == o.price; });
            if (it != levels.end()) it->second += o.remaining();
            else                    levels.push_back({o.price, o.remaining()});
        }
        for (auto& o : valid) pq.push(o);     
        return levels;
    }

public:
    Order submit(Order& order);
    bool cancel(uint64_t id);
    string best_bid_str();
    string best_ask_str();
    double spread();
    size_t bid_depth();
    size_t ask_depth();
    size_t trade_count() const;
    const vector<Trade>& trades() const;
    void display(ostream& out = cout);
    void show_trades(ostream& out = cout) const;
};

#endif 