#pragma once

#include <string>
#include <stdexcept>

#include "markets/market.h"


namespace TradingBot {

    enum OrderDirection {
        ORDER_DIRECTION_BUY = 0,
        ORDER_DIRECTION_SELL = 1
    };

    std::string toString(OrderDirection direction);
    std::string toString(CandleTimeDelta delta);

} // namespace TradingBot
