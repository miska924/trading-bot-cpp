#pragma once

#include <vector>
#include <optional>

#include "markets/market.h"


namespace TradingBot {

    class Feature {
    public:
        virtual ~Feature() = default;
        virtual std::optional<double> operator()(const Helpers::VectorView<Candle>& candles) const = 0;
    };

} // namespace TradingBot
