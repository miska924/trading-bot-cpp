#pragma once

#include "strategies/strategy.h"


namespace TradingBot {

    const double DUMMY_STRATEGY_BUY_AMOUNT = 1.0;

    class DummyStrategy : public Strategy {
    public:
        DummyStrategy(Market* _market);
        virtual void step() override;
    };

} // namespace TradingBot