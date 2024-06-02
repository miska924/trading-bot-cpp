#pragma once

#include <optional>
#include <string>
#include <time.h>

#include "markets/market.h"


namespace TradingBot {

    const int TEST_CANDLES_TIMEDELTA = 15 * 60;

    const char COMMA = ',';
    const double DEFAULT_FEE = 0.002;

    Candle readCSVCandle(std::string line);
    std::vector<Candle> readCSVFile(std::string dataFileName);

    class BacktestMarket : public Market {
    public:
        BacktestMarket(const Helpers::VectorView<Candle>& candles, time_t candleTimeDelta, bool saveHistory = true, bool verbose = false);
        ~BacktestMarket() = default;
        time_t time() const override;
        virtual bool order(Order order) override;
        virtual bool update() override;
        virtual Helpers::VectorView<Candle> getCandles() const override;
        virtual const Order& getLastOrder() const override;
        virtual bool finished() const override;
        void finish();
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
        // int candleTimeDelta;
    };

} // namespace TradingBot
