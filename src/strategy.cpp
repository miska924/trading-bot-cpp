#include "strategies/strategy.h"


namespace TradingBot {

    const ParamSet& Strategy::getParamSet() const {
        return paramSet;
    }

    ParamSet Strategy::getDefaultParamSet() const {
        return {};
    }

    ParamSet Strategy::getMinParamSet() const {
        return {};
    }

    ParamSet Strategy::getMaxParamSet() const {
        return {};
    }

    bool Strategy::checkParamSet(const ParamSet& paramSet) const {
        return true;
    }

    void Strategy::run() {
        do {
            step();
        } while (market->update());
    }

} // namespace TradingBot
