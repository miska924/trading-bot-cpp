#pragma once

#include <time.h>
#include <string>
#include <optional>

#include "markets/market.h"


namespace TradingBot {

    const char COMMA = ',';
    const double DEFAULT_FEE = 0.002;

    Candle readCSVCandle(std::string line);
    std::vector<Candle> readCSVFile(std::string dataFileName);

    class BacktestMarket : public Market {
    public:
        BacktestMarket(const Helpers::VectorView<Candle>& candles, bool saveHistory = true);
        ~BacktestMarket() = default;
        time_t time() const override;
        virtual bool order(Order order) override;
        virtual bool update() override;
        virtual Helpers::VectorView<Candle> getCandles() const override;
        void finish();
        void restart();
        double getFitness() const;

    private:
        Helpers::VectorView<Candle> candles;
        int current = -1;
        bool saveHistory;

        double maxBalance = 0;
        double maxDrawdown = 0;
        double sumSquaredDrawdown = 0;
    };

} // namespace TradingBot
