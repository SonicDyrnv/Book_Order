C++ Exchange Matching Engine

Description:
A modular C++ exchange matching engine implementing price-time priority, limit/market orders, lazy cancellation, and multi-symbol order book management.

Features:

Limit Buy Orders
Limit Sell Orders
Market Buy Orders
Market Sell Orders
Price-Time Priority Matching
Trade Execution Logging
Lazy O(1) Order Cancellation
Order Modification (Cancel-Replace)
Best Bid / Best Ask Calculation
Spread Calculation
Multi-symbol Support

Project Files:

Order.h
OrderBook.h
OrderBook.cpp
Exchange.h
Exchange.cpp
main.cpp

Data Structures Used:

priority_queue
unordered_map
unordered_set
vector

Example Operations:

Exchange ex;
ex.limit_buy("AAPL",100.00,200);
ex.limit_sell("AAPL",101.50,100);
ex.market_sell("AAPL",300);
ex.cancel(8);
ex.modify(4,99.80,200);

Time Complexity:

Insert Order: O(log n)
Match Order: O(k log n)
Cancel Order: O(1)
Best Bid/Ask: O(1)
