#pragma once

#include "strategies/strategy.h"


namespace TradingBot {

    const double DUMMY_STRATEGY_TRADE_AMOUNT = 1.0;

    class DummyStrategy : public Strategy {
    public:
        DummyStrategy(Market* _market);
        virtual void step() override;
    };

} // namespace TradingBot
