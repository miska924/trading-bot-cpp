#include "strategies/dummy_strategy.h"


namespace TradingBot {

    DummyStrategy::DummyStrategy(Market* _market) {
        market = _market;
    }

    void DummyStrategy::step() {
        market->order({
            .side = OrderSide::BUY,
            .amount = DUMMY_STRATEGY_BUY_AMOUNT
        });
    }

} // namespace TradingBot
