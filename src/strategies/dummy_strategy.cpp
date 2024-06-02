#include "strategies/dummy_strategy.h"

#include <random>


namespace TradingBot {

    DummyStrategy::DummyStrategy(Market* _market) {
        market = _market;
    }

    void DummyStrategy::step() {
        if (market->getBalance().assetB != 0) {
            market->order({.side = OrderSide::RESET});
        }

        market->order({.side = OrderSide::BUY, .amount = DUMMY_STRATEGY_TRADE_AMOUNT});

        // if (market->getCandles().size() % 2) {
        //     // market->order({.side = OrderSide::SELL, .amount = DUMMY_STRATEGY_TRADE_AMOUNT});
        // } else {
        //     market->order({.side = OrderSide::BUY, .amount = DUMMY_STRATEGY_TRADE_AMOUNT});
        // }
    }

} // namespace TradingBot
