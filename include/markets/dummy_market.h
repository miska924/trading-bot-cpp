#pragma once

#include "markets/market.h"


namespace TradingBot {

    class DummyMarket : public Market {
    public:
        virtual bool order(Order order) override;
        virtual bool update() override;
    };

} // namespace TradingBot
