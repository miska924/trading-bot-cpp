#pragma once

#include "strategies/strategy.h"


namespace TradingBot {

    class DummyStrategy : public Strategy {
    public:
        DummyStrategy(const ParamSet& paramSet);

        bool checkParamSet(const ParamSet& paramSet) const override;
        Signal step(bool newCandle) override;
    };

} // namespace TradingBot
