#include "OrderBook.h"

void OrderBook::clean_bids() {
    while (!bids_.empty() && cancelled_.count(bids_.top().id)) {
        cancelled_.erase(bids_.top().id);
        bids_.pop();
    }
}

void OrderBook::clean_asks() {
    while (!asks_.empty() && cancelled_.count(asks_.top().id)) {
        cancelled_.erase(asks_.top().id);
        asks_.pop();
    }
}

void OrderBook::emit_trade(const Order& buyer, const Order& seller, double price, uint64_t qty) {
    trades_.push_back({++trade_seq_, buyer.id, seller.id, price, qty});
}

void OrderBook::match_buy(Order& buyer) {
    while (buyer.remaining() > 0) {
        clean_asks();
        if (asks_.empty()) break;

        Order ask = asks_.top();
        asks_.pop();

        if (buyer.type == OrderType::LIMIT && ask.price > buyer.price) {
            asks_.push(ask);
            break;
        }

        uint64_t fill = min(buyer.remaining(), ask.remaining());
        double   px   = ask.price;          

        buyer.filled_qty += fill;
        ask.filled_qty   += fill;
        emit_trade(buyer, ask, px, fill);

        if (ask.remaining() > 0)
            asks_.push(ask);               
        else
            resting_.erase(ask.id);         
    }
}

void OrderBook::match_sell(Order& seller) {
    while (seller.remaining() > 0) {
        clean_bids();
        if (bids_.empty()) break;

        Order bid = bids_.top();
        bids_.pop();

        if (seller.type == OrderType::LIMIT && bid.price < seller.price) {
            bids_.push(bid);
            break;
        }

        uint64_t fill = min(seller.remaining(), bid.remaining());
        double   px   = bid.price;

        seller.filled_qty += fill;
        bid.filled_qty    += fill;
        emit_trade(bid, seller, px, fill);

        if (bid.remaining() > 0)
            bids_.push(bid);
        else
            resting_.erase(bid.id);
    }
}

Order OrderBook::submit(Order& order) {
    if (order.side == Side::BUY)  match_buy(order);
    else                          match_sell(order);

    if (order.type == OrderType::LIMIT && order.remaining() > 0) {
        if (order.side == Side::BUY) bids_.push(order);
        else                        asks_.push(order);
        resting_.insert(order.id);
    }
    return order;
}

bool OrderBook::cancel(uint64_t id) {
    if (!resting_.count(id)) return false;
    resting_.erase(id);
    cancelled_.insert(id);
    return true;
}

string OrderBook::best_bid_str() {
    clean_bids();
    if (bids_.empty()) return "---";
    ostringstream o;
    o << fixed << setprecision(2) << bids_.top().price
      << " x" << bids_.top().remaining();
    return o.str();
}

string OrderBook::best_ask_str() {
    clean_asks();
    if (asks_.empty()) return "---";
    ostringstream o;
    o << fixed << setprecision(2) << asks_.top().price
      << " x" << asks_.top().remaining();
    return o.str();
}

double OrderBook::spread() {
    clean_bids(); clean_asks();
    if (bids_.empty() || asks_.empty()) return 0.0;
    return asks_.top().price - bids_.top().price;
}

size_t OrderBook::bid_depth()   { clean_bids(); return bids_.size(); }
size_t OrderBook::ask_depth()   { clean_asks(); return asks_.size(); }
size_t OrderBook::trade_count() const { return trades_.size(); }
const vector<Trade>& OrderBook::trades() const { return trades_; }

void OrderBook::display(ostream& out) {
    auto bids = aggregate(bids_);
    auto asks = aggregate(asks_);

    sort(bids.begin(), bids.end(),
         [](auto& a, auto& b) { return a.first > b.first; });
    sort(asks.begin(), asks.end(),
         [](auto& a, auto& b) { return a.first < b.first; });

    ostringstream bb_s, ba_s;
    if (!bids.empty())
        bb_s << fixed << setprecision(2)
             << bids[0].first << " x" << bids[0].second;
    if (!asks.empty())
        ba_s << fixed << setprecision(2)
             << asks[0].first << " x" << asks[0].second;
    double sp = (bids.empty() || asks.empty()) ? 0.0
                 : asks[0].first - bids[0].first;

    out << setfill(' ')
        << "\n+" << string(56, '-') << "+\n"
        << "|  Best Bid: " << left  << setw(18) << bb_s.str()
        << "| Best Ask: " << setw(18) << ba_s.str() << "|\n"
        << "|  Spread: "   << fixed << setprecision(2)
        << setw(10) << sp
        << "  | Levels: "
        << setw(3) << bids.size() << " bid / "
        << setw(3) << asks.size() << " ask  |\n"
        << "+" << string(56, '-') << "+\n"
        << "| " << setw(24) << left  << "BIDS" << " | "
        << setw(24) << right << "ASKS" << " |\n"
        << "| " << string(22, '-') << " | "
        << string(22, '-') << " |\n";

    size_t rows = max(bids.size(), asks.size());
    for (size_t i = 0; i < rows; ++i) {
        ostringstream L, R;
        if (i < bids.size())
            L << fixed << setprecision(2)
              << bids[i].first << " x" << bids[i].second;
        if (i < asks.size())
            R << "x" << asks[i].second << " "
              << fixed << setprecision(2) << asks[i].first;
        out << "| " << setw(22) << left  << L.str() << " | "
                   << setw(22) << right << R.str() << " |\n";
    }
    out << "+" << string(56, '-') << "+\n";
}

void OrderBook::show_trades(ostream& out) const {
    out << setfill(' ')
        << "\n+" << string(56, '-') << "+\n"
        << "|  TRADE LOG  (" << trade_count() << " trades)\n"
        << "+" << string(56, '-') << "+\n";
    for (auto& t : trades())
        out << "| " << t.str() << "\n";
    out << "+" << string(56, '-') << "+\n";
}