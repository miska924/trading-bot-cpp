#include "markets/dummy_market.h"


namespace TradingBot {

    bool DummyMarket::order(Order order) {
        saveOrder(order);
        return true;
    }

    bool DummyMarket::update() {
        return true;
    }

    bool DummyMarket::finished() const {
        return orderHistory.size();
    }

    Helpers::VectorView<Candle> DummyMarket::getCandles() const {
        return Helpers::VectorView<Candle>(candles);
    }

} // namespace TradingBot
