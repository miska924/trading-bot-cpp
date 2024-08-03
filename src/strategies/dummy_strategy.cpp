#include "strategies/dummy_strategy.h"


namespace TradingBot {

    Signal DummyStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        if (market->getBalance().assetB != 0) {
            return {
                .reset = true
            };
        }

        return {
            .order = 1
        };
    }

} // namespace TradingBot
