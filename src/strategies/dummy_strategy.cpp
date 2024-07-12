#include "strategies/dummy_strategy.h"


namespace TradingBot {

    DummyStrategy::DummyStrategy(Market* _market) {
        market = _market;
    }

    void DummyStrategy::step() {
        if (market->getBalance().assetB != 0) {
            std::cerr << "RESET" << std::endl;
            market->order({.side = OrderSide::RESET});
            return;
        }

        market->order({
            .side = OrderSide::BUY,
            .amount = DUMMY_STRATEGY_BUY_AMOUNT
        });
    }

} // namespace TradingBot
