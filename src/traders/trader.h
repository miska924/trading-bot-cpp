#pragma once

#include <memory>

#include "strategies/strategy.h"


namespace TradingBot {

    class Trader {
    public:
        Trader(Strategy* strategy, Market* market);
        virtual ~Trader() = default;

        void run();
        virtual void step(bool newCandle) = 0;
    protected:
        Strategy* strategy;
        Market* market;
        MarketInfo marketInfo;
    };

} // namespace TradingBot
