#pragma once

#include <optional>
#include <vector>

#include "markets/market.h"


namespace TradingBot {

    class Feature {
    public:
        virtual ~Feature() = default;
        virtual double operator()(const Helpers::VectorView<Candle>& candles, bool incremental = false) = 0;
    };

} // namespace TradingBot
