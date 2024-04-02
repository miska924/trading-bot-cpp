#include "strategies/dummy_strategy.h"

namespace trading_bot {

    void DummyStrategy::run() {
        while (market->update()) {
            market->order({
                .side = OrderSide::BUY,
                .amount = DUMMY_STRATEGY_BUY_AMOUNT
            });
        }
    }
} // trading_bot
