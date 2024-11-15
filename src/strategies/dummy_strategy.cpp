#include "strategies/dummy_strategy.h"


namespace TradingBot {

    DummyStrategy::DummyStrategy(const ParamSet& paramSet): Strategy(paramSet) {}

    bool DummyStrategy::checkParamSet(const ParamSet& paramSet) const {
        return true;
    }

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
