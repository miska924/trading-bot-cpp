#pragma once

#include "markets/market.h"


namespace TradingBot {

    class Strategy {
    public:
        virtual ~Strategy() = default;
        virtual void run() = 0;
    };

} // namespace TradingBot
