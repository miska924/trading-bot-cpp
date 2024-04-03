#include "strategies/dummy_strategy.h"

namespace TradingBot {

    void DummyStrategy::run() {
        while (market->update()) {
            market->order({
                .side = OrderSide::BUY,
                .amount = DUMMY_STRATEGY_BUY_AMOUNT
            });
        }
    }
} // TradingBot
