#include "Exchange.h"

void print_fill(const Order& o) {
    cout << setfill(' ')
         << "   -> Filled " << o.filled_qty << "/" << o.quantity;
    if (o.filled_qty == 0)
        cout << "  -> resting in book";
    else if (o.remaining() > 0)
        cout << "  -> partial fill, " << o.remaining() << " resting";
    else
        cout << "  -> fully filled";
    cout << endl;
}

int main() {
    Exchange ex;

    cout << "\n" << string(58, '-') << endl
         << "PHASE 1 -- Seed AAPL and GOOG order books"<<endl
         << string(58, '-') << endl;

    print_fill(ex.limit_sell("AAPL", 101.50, 100));   // #1
    print_fill(ex.limit_sell("AAPL", 101.00, 200));   // #2
    print_fill(ex.limit_sell("AAPL", 100.75, 150));   // #3
    print_fill(ex.limit_buy ("AAPL",  99.50, 300));   // #4
    print_fill(ex.limit_buy ("AAPL",  99.75, 200));   // #5
    print_fill(ex.limit_buy ("AAPL", 100.00, 100));   // #6

    print_fill(ex.limit_sell("GOOG", 155.00,  80));   // #7
    print_fill(ex.limit_sell("GOOG", 154.50, 120));   // #8
    print_fill(ex.limit_buy ("GOOG", 153.00, 200));   // #9
    print_fill(ex.limit_buy ("GOOG", 153.50, 150));   // #10

    ex.show_book("AAPL");
    ex.show_book("GOOG");

    cout << "\n" << string(58, '-') << endl
         << "PHASE 2 -- Limit buy on AAPL crosses the spread"<<endl
         << string(58, '-') << endl;

    print_fill(ex.limit_buy("AAPL", 100.75, 200));
    ex.show_book("AAPL");
    ex.show_trades("AAPL");

    cout << "\n" << string(58, '-') << endl
         << "PHASE 3 -- O(1) lazy cancel on GOOG"<<endl
         << string(58, '-') << endl;

    ex.cancel(8);     // remove GOOG ask @154.50
    ex.cancel(999);   // doesn't exist
    ex.show_book("GOOG");

    cout << "\n" << string(58, '-') << endl
         << "PHASE 4 -- Modify AAPL #4: 99.50 x300 -> 99.80 x200"<<endl
         << string(58, '-') << endl;

    auto result4 = ex.modify(4, 99.80, 200);
    if (result4.first) print_fill(result4.second);
    ex.show_book("AAPL");

    cout << "\n" << string(58, '-') << endl
         << "PHASE 5 -- Market sell 300 sweeps AAPL bids"<<endl
         << string(58, '-') << endl;

    print_fill(ex.market_sell("AAPL", 300));
    ex.show_book("AAPL");
    ex.show_trades("AAPL");

    cout << "\n" << string(58, '-') << endl
         << "PHASE 6 -- Price-time priority on GOOG"<<endl
         << string(58, '-') << endl;

    print_fill(ex.limit_buy("GOOG", 153.50, 60));    // #13
    print_fill(ex.limit_buy("GOOG", 153.50, 40));    // #14

    cout << "\n  Market sell 100 -> should fill #10 (oldest at 153.50):\n";

    print_fill(ex.market_sell("GOOG", 100));

    cout << "\n  Market sell 100 -> fill #10(50 left) then #13(50):\n";
    print_fill(ex.market_sell("GOOG", 100));

    ex.show_trades("GOOG");
    ex.show_stats("AAPL");
    ex.show_stats("GOOG");

    return 0;
}