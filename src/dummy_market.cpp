#include "markets/dummy_market.h"


namespace trading_bot {

    bool DummyMarket::order(Order order) {
        saveOrder(order);
        return true;
    }

    bool DummyMarket::update() {
        return false;
    }

} // trading_bot
