#pragma once

#include <time.h>
#include <string>
#include <optional>

#include "markets/market.h"


namespace trading_bot {

    const char COMMA = ',';
    const size_t DEFAULT_RANDOM_BACKTEST_MARKET_SIZE = 100;

    class BacktestMarket : public Market {
    public:
        BacktestMarket(size_t size = DEFAULT_RANDOM_BACKTEST_MARKET_SIZE);
        BacktestMarket(std::string dataFileName);
        ~BacktestMarket() = default;
        time_t time() const override;
        virtual bool order(Order order) override;
        void finish();
        const std::vector<Candle>* allCandles() const;
    private:
        time_t marketTime = 0;
        std::vector<Candle> candles;
    };

}
