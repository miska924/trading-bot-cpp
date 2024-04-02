#pragma once

#include "markets/market.h"


namespace trading_bot {

    class Strategy {
    public:
        virtual ~Strategy() = default;
        virtual void run() = 0;
    };

} // trading_bot
