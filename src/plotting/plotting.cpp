#include "plotting/plotting.h"

#include <assert.h>
#include <cstdlib>


namespace TradingBot {

    bool checkGnuplot() {
        // return false;
        return std::system("gnuplot -V > /dev/null 2>&1") == 0;
    }
    
    void plot(
        std::string fileName,
        const Helpers::VectorView<TradingBot::Candle>& candles,
        const std::vector<TradingBot::Order>& orders,
        const std::vector<TradingBot::Balance>& balances,
        const std::vector<std::vector<std::pair<time_t, double>>>& additional,
        bool logBalance
    ) {
        if (!checkGnuplot()) {
            return;
        }

        std::vector<double> candleCloses, candleTimes;
        candleCloses.reserve(candles.size());
        candleTimes.reserve(candles.size());
        for (size_t i = 0; i < candles.size(); ++i) {
            candleCloses.push_back(candles[i].close);
            candleTimes.push_back(candles[i].time);
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

        std::vector<std::pair<std::vector<time_t>, std::vector<double>>> additionalPlots;
        for (int i = 0; i < additional.size(); ++i) {
            additionalPlots.push_back({{}, {}});
            for (int j = 0; j < additional[i].size(); ++j) {
                additionalPlots.back().first.push_back(additional[i][j].first);
                additionalPlots.back().second.push_back(additional[i][j].second);
            }
        }

        auto fig = matplot::figure(true);
        auto ax1 = fig->add_subplot(2, 1, 1);

        ax1->hold(matplot::on);
        ax1->plot(candleTimes, candleCloses);

        std::cerr << "PLOTS: " << additionalPlots.size() << std::endl;
        for (int i = 0; i < additionalPlots.size(); ++i) {
            // for (int j = 0; j < additionalPlots[i].second.size(); ++j) {
            //     std::cerr << additionalPlots[i].first[j] << " ";
            // }
            // std::cerr << std::endl;

            auto plot = ax1->plot(additionalPlots[i].first, additionalPlots[i].second);
            plot->color("#FFAA00");
        }

        auto buy = ax1->scatter(buyOrderTimes, buyOrderPrices, 10);
        buy->marker_face_color("#00AA00");
        buy->color("none");

        auto sell = ax1->scatter(sellOrderTimes, sellOrderPrices, 10);
        sell->marker_face_color("red");
        sell->color("none");

        auto reset = ax1->scatter(resetOrderTimes, resetOrderPrices, 5);
        reset->marker_face_color("black");
        reset->color("none");

        auto ax2 = fig->add_subplot(2, 1, 2);
        ax1->shared_from_this();
        ax2->hold(matplot::on);
        if (logBalance) {
            ax2->loglog(balanceTimes, balanceHistory);
        } else {
            ax2->plot(balanceTimes, balanceHistory);
        }

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

    void heatmap(
        std::string fileName,
        const Helpers::MultidimVector<double>& values
    ) {
        assert(values.getShape().size() == 2);

        if (!checkGnuplot()) {
            return;
        }

        std::vector<std::vector<double>> v;
        for (size_t i = 0; i < values.getShape()[0]; ++i) {
            v.push_back(std::vector<double>(values.getShape()[1]));
            for (size_t j = 0; j < values.getShape()[1]; ++j) {
                v[i][j] = values[{i, j}] == -1e18 ? std::nan("") : values[{i, j}];
            }
        }

        auto fig = matplot::figure(true);
        auto ax = fig->current_axes();
        ax->heatmap(v);
        ax->colormap(v);

        fig->size(DEFAULT_PLOT_HEIGHT, DEFAULT_PLOT_HEIGHT);
        fig->save(fileName);
    }

} // namespace TradingBot
