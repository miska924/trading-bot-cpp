#include "markets/market.h"

#include <chrono>
#include <time.h>


namespace trading_bot {

    time_t Market::time() const {
        return std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
        );
    }

    const std::vector<Order>& Market::history() const {
        return orderHistory;
    }

    void Market::saveOrder(Order order) {
        orderHistory.push_back(order);
    }

    bool DummyMarket::order(Order order) {
        saveOrder(order);
        return true;
    }

    bool DummyMarket::update() {
        return false;
    }

} // trading_bot
