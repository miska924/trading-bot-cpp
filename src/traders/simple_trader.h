#pragma once

#include "traders/trader.h"


namespace TradingBot {

    class SimpleTrader : public Trader {
    public:
        SimpleTrader(Strategy* strategy, Market* market);
        virtual ~SimpleTrader() override = default;

        virtual void step(bool newCandle) override;
    };

} // namespace TradingBot
