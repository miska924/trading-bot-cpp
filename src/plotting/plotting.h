#pragma once

#include <matplot/matplot.h>

#include "markets/market.h"
#include "strategies/strategy.h"
#include "helpers/multidim_vector.h"


namespace TradingBot {

    const int DEFAULT_PLOT_WIDTH = 1500;
    const int DEFAULT_PLOT_HEIGHT = 600;
    const float HIGHT_MARGIN = 0.1;
    const float WIDTH_MARGIN = (HIGHT_MARGIN * DEFAULT_PLOT_HEIGHT) / DEFAULT_PLOT_WIDTH;
    
    void plot(
        std::string fileName,
        const Helpers::VectorView<TradingBot::Candle>& candles,
        const std::vector<TradingBot::Order>& orders,
        const std::vector<TradingBot::Balance>& balances
    );

    void heatmap(
        std::string fileName,
        const Helpers::MultidimVector<double>& values
    );

} // namespace TradingBot
