#pragma once

#include "markets/market.h"


namespace trading_bot {

    const double DUMMY_STRATEGY_BUY_AMOUNT = 1.0;

    class Strategy {
    public:
        virtual ~Strategy() = default;
        virtual void run() = 0;
    };

    class DummyStrategy : public Strategy {
    public:
        DummyStrategy(Market* market) : market(market) {};
        virtual void run() override;
    protected:
        Market* market;
    };
}