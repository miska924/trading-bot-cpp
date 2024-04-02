#pragma once

#include "markets/market.h"


namespace trading_bot {

    class DummyMarket : public Market {
    public:
        virtual bool order(Order order) override;
        virtual bool update() override;
    };

} // trading_bot
