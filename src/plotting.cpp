#include "plotting/plotting.h"

#include <cstdlib>


namespace TradingBot {

    bool checkGnuplot() {
        return std::system("gnuplot -V > /dev/null 2>&1") == 0;
    }
    
    void plot(
        std::string fileName,
        std::vector<TradingBot::Candle> candles,
        std::vector<TradingBot::Order> orders
    ) {
        if (!checkGnuplot()) {
            return;
        }

        std::vector<double> candleCloses, candleTimes;
        for (const TradingBot::Candle& candle : candles) {
            candleCloses.push_back(candle.close);
            candleTimes.push_back(candle.time);
        }

        std::vector<double> buyOrderPrices, buyOrderTimes, sellOrderPrices, sellOrderTimes;
        for (const TradingBot::Order& order : orders) {
            if (order.side == TradingBot::OrderSide::BUY) {
                buyOrderPrices.push_back(order.price);
                buyOrderTimes.push_back(order.time);
            } else {
                sellOrderPrices.push_back(order.price);
                sellOrderTimes.push_back(order.time);
            }
        }

        auto f = matplot::figure(true);
        auto ax = matplot::axes(f);
        ax->position({
            WIDTH_MARGIN,
            HIGHT_MARGIN,
            float(1.0 - 2 * WIDTH_MARGIN),
            float(1.0 - 2 * HIGHT_MARGIN),
        });

        f->size(DEFAULT_PLOT_WIDTH, DEFAULT_PLOT_HEIGHT);
        ax->plot(candleTimes, candleCloses);

        ax->hold(matplot::on);

        auto buy = ax->scatter(buyOrderTimes, buyOrderPrices);
        buy->marker_face_color("#00AA00");
        buy->color("none");

        auto sell = ax->scatter(sellOrderTimes, sellOrderPrices);
        sell->marker_face_color("red");
        sell->color("none");

        f->save(fileName);
    }

} // namespace TradingBot
