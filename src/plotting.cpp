#include "plotting/plotting.h"

#include <cstdlib>


namespace TradingBot {

    bool checkGnuplot() {
        return std::system("gnuplot -V > /dev/null 2>&1") == 0;
    }
    
    void plot(
        std::string fileName,
        const std::vector<TradingBot::Candle>& candles,
        const std::vector<TradingBot::Order>& orders,
        const std::vector<TradingBot::Balance>& balances
    ) {
        if (!checkGnuplot()) {
            return;
        }

        std::vector<double> candleCloses, candleTimes;
        candleCloses.reserve(candles.size());
        candleTimes.reserve(candles.size());
        for (const TradingBot::Candle& candle : candles) {
            candleCloses.push_back(candle.close);
            candleTimes.push_back(candle.time);
        }

        std::vector<double> balanceHistory, balanceTimes;
        balanceHistory.reserve(balances.size());
        balanceTimes.reserve(balances.size());
        for (const TradingBot::Balance& balance : balances) {
            balanceHistory.push_back(balance.asAssetA());
            balanceTimes.push_back(balance.time);
        }

        std::vector<double> buyOrderPrices,
                            buyOrderTimes,
                            sellOrderPrices,
                            sellOrderTimes,
                            resetOrderPrices,
                            resetOrderTimes;
        buyOrderPrices.reserve(orders.size());
        buyOrderTimes.reserve(orders.size());
        sellOrderPrices.reserve(orders.size());
        sellOrderTimes.reserve(orders.size());
        resetOrderPrices.reserve(orders.size());
        resetOrderTimes.reserve(orders.size());
        for (const TradingBot::Order& order : orders) {
            if (order.side == TradingBot::OrderSide::BUY) {
                buyOrderPrices.push_back(order.price);
                buyOrderTimes.push_back(order.time);
            } else if (order.side == TradingBot::OrderSide::SELL) {
                sellOrderPrices.push_back(order.price);
                sellOrderTimes.push_back(order.time);
            } else if (order.side == TradingBot::OrderSide::RESET) {
                resetOrderPrices.push_back(order.price);
                resetOrderTimes.push_back(order.time);
            }
        }

        // Создаём фигуру и определяем размеры подграфов (2 ряда, 1 столбец, текущий подграф 1)
        auto fig = matplot::figure(true);

        auto ax1 = fig->add_subplot(2, 1, 1);
        ax1->hold(matplot::on);
        ax1->plot(candleTimes, candleCloses);


        auto reset = ax1->scatter(resetOrderTimes, resetOrderPrices);
        reset->marker_face_color("gray");
        reset->color("none");

        auto buy = ax1->scatter(buyOrderTimes, buyOrderPrices);
        buy->marker_face_color("#00AA00");
        buy->color("none");

        auto sell = ax1->scatter(sellOrderTimes, sellOrderPrices);
        sell->marker_face_color("red");
        sell->color("none");

        auto ax2 = fig->add_subplot(2, 1, 2);
        ax1->shared_from_this();
        ax2->hold(matplot::on);
        ax2->plot(balanceTimes, balanceHistory);

        ax1->position({
            WIDTH_MARGIN,
            float((1.0 - 3 * HIGHT_MARGIN) / 2 + 2 * HIGHT_MARGIN),
            float(1.0 - 2 * WIDTH_MARGIN),
            float((1.0 - 3 * HIGHT_MARGIN) / 2)
        });
        ax2->position({
            WIDTH_MARGIN,
            HIGHT_MARGIN,
            float(1.0 - 2 * WIDTH_MARGIN),
            float((1.0 - 3 * HIGHT_MARGIN) / 2)
        });

        fig->size(DEFAULT_PLOT_WIDTH, DEFAULT_PLOT_HEIGHT);
        fig->save(fileName);

    }

} // namespace TradingBot
