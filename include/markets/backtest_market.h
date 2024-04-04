#pragma once

#include <time.h>
#include <string>
#include <optional>

#include "markets/market.h"


namespace TradingBot {

    const char COMMA = ',';
    const size_t DEFAULT_RANDOM_BACKTEST_MARKET_SIZE = 100;

    Candle readCSVCandle(std::string line);

    class BacktestMarket : public Market {
    public:
        BacktestMarket(size_t size = DEFAULT_RANDOM_BACKTEST_MARKET_SIZE);
        BacktestMarket(std::string dataFileName);
        ~BacktestMarket() = default;
        time_t time() const override;
        virtual bool order(Order order) override;
        virtual bool update() override;
        void finish();
    private:
        std::vector<Candle> futureCandles;
    };

} // namespace TradingBot
