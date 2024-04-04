#include "markets/dummy_market.h"


namespace TradingBot {

    bool DummyMarket::order(Order order) {
        saveOrder(order);
        return true;
    }

    bool DummyMarket::update() {
        return false;
    }

} // namespace TradingBot
