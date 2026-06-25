#include "Exchange.h"

string Exchange::fmt_id(uint64_t id) {
    ostringstream o;
    o << setw(4) << setfill('0') << id;
    return o.str();
}

Order Exchange::make(Side side, OrderType type, double price, uint64_t qty) {
    uint64_t ts = static_cast<uint64_t>(
        chrono::steady_clock::now()
            .time_since_epoch().count());
    return Order{++next_id_, side, type, price, qty, ts};
}

Order Exchange::do_submit(const string& sym, Side side, OrderType type, double price, uint64_t qty) {
    Order order = make(side, type, price, qty);
    meta_[order.id] = {side, sym};
    Order result = books_[sym].submit(order);
    if (result.remaining() == 0)
        meta_.erase(result.id);       
    return result;
}

Order Exchange::limit_buy(const string& sym, double price, uint64_t qty) {
    cout << setfill(' ')
         << " [" << sym << "] Limit  Buy  : " << fmt_id(next_id_ + 1)
         << "  at" << fixed << setprecision(2) << price
         << " x" << qty << endl;
    return do_submit(sym, Side::BUY, OrderType::LIMIT, price, qty);
}

Order Exchange::limit_sell(const string& sym, double price, uint64_t qty) {
    cout << setfill(' ')
         << " [" << sym << "] Limit Sell  : " << fmt_id(next_id_ + 1)
         << "  at" << fixed << setprecision(2) << price
         << " x" << qty << endl;
    return do_submit(sym, Side::SELL, OrderType::LIMIT, price, qty);
}

Order Exchange::market_buy(const string& sym, uint64_t qty) {
    cout << setfill(' ')
         << " [" << sym << "] Market Buy  : " << fmt_id(next_id_ + 1)
         << "  x" << qty << "\n";
    Order order = make(Side::BUY, OrderType::MARKET, 0.0, qty);
    return books_[sym].submit(order);
}

Order Exchange::market_sell(const string& sym, uint64_t qty) {
    cout << setfill(' ')
         << " [" << sym << "] Market Sell #" << fmt_id(next_id_ + 1)
         << "  x" << qty << endl;
    Order order = make(Side::SELL, OrderType::MARKET, 0.0, qty);
    return books_[sym].submit(order);
}

bool Exchange::cancel(uint64_t id) {
    auto it = meta_.find(id);
    if (it == meta_.end()) {
        cout << setfill(' ')
             << " Cancel : " << fmt_id(id) << "  Not Found"<< endl;
        return false;
    }
    bool ok = books_[it->second.symbol].cancel(id);
    if (ok) meta_.erase(it);
    cout << setfill(' ')
         << " Cancel : " << fmt_id(id) << "  "
         << (ok ? "Ok " : "NOT RESTING") << endl;
    return ok;
}

pair<bool, Order> Exchange::modify(uint64_t id, double new_price, uint64_t new_qty) {
    auto it = meta_.find(id);
    if (it == meta_.end()) {
        cout << setfill(' ')
             << " Modify : " << fmt_id(id) << "  Not Found"<<endl;
        return {false, {}};
    }

    Side   side = it->second.side;
    string sym  = it->second.symbol;

    if (!books_[sym].cancel(id)) {
        cout << setfill(' ')
             << " Modify :" << fmt_id(id) << "  Not Resting"<<endl;
        return {false, {}};
    }
    meta_.erase(it);

    uint64_t new_id = next_id_ + 1;
    cout << setfill(' ') << " Modify : " << fmt_id(id) << " -> : " << fmt_id(new_id)
         << "  at" << fixed << setprecision(2) << new_price<< " x" << new_qty << "  (cancel-replace)"<<endl;

    Order result = do_submit(sym, side, OrderType::LIMIT, new_price, new_qty);
    return {true, result};
}

void Exchange::show_book(const string& sym) {
    cout << setfill(' ') << endl <<" --- " << sym << " --- ";
    books_[sym].display(cout);
}

void Exchange::show_trades(const string& sym) {
    cout << setfill(' ') << endl<<"  --- " << sym << " Trades ---";
    books_[sym].show_trades(cout);
}

void Exchange::show_stats(const string& sym) {
    OrderBook& b = books_[sym];
    cout << setfill(' ') <<endl << "-- " << sym << " Stats :-:-:-: "<<endl
         << "  Open bids    : " << b.bid_depth()  << endl
         << "  Open asks    : " << b.ask_depth()  << endl
         << "  Total trades : " << b.trade_count() << endl
         << "  Best bid     : " << b.best_bid_str() << endl
         << "  Best ask     : " << b.best_ask_str() << endl
         << "  Spread       : " << fixed << setprecision(2)
         << b.spread() << "\n"
         << ":-:-:-:-:-;-:-;-:-:-:"<<endl;
}
