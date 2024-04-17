#pragma once

#include "markets/market.h"
#include "markets/tinkoff/tinkoff_api.h"


namespace TradingBot {

    class TinkoffMarket : public Market {
    public:
        TinkoffMarket();
        ~TinkoffMarket() = default;
    };

} // namespace TradingBot
