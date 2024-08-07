#pragma once

#include <matplot/matplot.h>

#include "helpers/multidim_vector.h"
#include "markets/market.h"
#include "strategies/strategy.h"


namespace TradingBot {

    const int DEFAULT_PLOT_WIDTH = 1500;
    const int DEFAULT_PLOT_HEIGHT = 600;
    const float HIGHT_MARGIN = 0.1;
    const float WIDTH_MARGIN = (HIGHT_MARGIN * DEFAULT_PLOT_HEIGHT) / DEFAULT_PLOT_WIDTH;
    
    void plot(
        std::string fileName,
        const Helpers::VectorView<TradingBot::Candle>& candles,
        const std::vector<TradingBot::Order>& orders,
        const std::vector<TradingBot::Balance>& balances,
        const std::vector<std::vector<std::pair<time_t, double>>>& additional = {},
        bool logBalance = false
    );

    void heatmap(
        std::string fileName,
        const Helpers::MultidimVector<double>& values
    );

} // namespace TradingBot
