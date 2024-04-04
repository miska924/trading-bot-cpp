#include <matplot/matplot.h>

#include "markets/market.h"


namespace TradingBot {

    const int DEFAULT_PLOT_WIDTH = 1500;
    const int DEFAULT_PLOT_HEIGHT = 600;
    const float HIGHT_MARGIN = 0.1;
    const float WIDTH_MARGIN = (HIGHT_MARGIN * DEFAULT_PLOT_HEIGHT) / DEFAULT_PLOT_WIDTH;
    
    void plot(
        std::string fileName,
        std::vector<TradingBot::Candle> candles,
        std::vector<TradingBot::Order> orders,
        std::vector<TradingBot::Balance> balances
    );

} // namespace TradingBot
