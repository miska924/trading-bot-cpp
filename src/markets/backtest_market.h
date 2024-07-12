#pragma once

#include <optional>
#include <string>
#include <time.h>

#include "markets/market.h"


namespace TradingBot {

    const char COMMA = ',';
    const double DEFAULT_FEE = 0.002;

    Candle readCSVCandle(std::string line);
    std::vector<Candle> readCSVFile(std::string dataFileName);

    class BacktestMarket : public Market {
    public:
        BacktestMarket(const Helpers::VectorView<Candle>& candles, bool saveHistory = true, bool verbose = false);
        ~BacktestMarket() = default;
        time_t time() const override;
        virtual bool order(Order order) override;
        virtual bool update() override;
        virtual Helpers::VectorView<Candle> getCandles() const override;
        virtual const Order& getLastOrder() const override;
        void finish();
        virtual bool finished() const override;
        void restart();
        double getFitness() const;

    private:
        Helpers::VectorView<Candle> candles;
        int current = -1;
        bool saveHistory;
        bool verbose;
        Order lastOrder = {.side = OrderSide::RESET};

        double maxBalance = 0;
        double maxDrawdown = 0;
        double sumSquaredDrawdown = 0;
    };

} // namespace TradingBot
