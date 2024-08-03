#pragma once

#include "strategies/strategy.h"


namespace TradingBot {

    class DummyStrategy : public Strategy {
    public:
        DummyStrategy() = default;
        virtual Signal step(bool newCandle) override;
    };

} // namespace TradingBot
