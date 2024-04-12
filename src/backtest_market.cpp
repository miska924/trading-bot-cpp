#include "markets/backtest_market.h"

#include <fstream>
#include <sstream>
#include <random>
#include <optional>
#include <iostream>
#include <algorithm>


namespace TradingBot {

    Candle readCSVCandle(std::string line) {
        std::istringstream iss(line);
        std::string token;
        Candle candle;

        std::getline(iss, token, ','); // skip string time

        std::getline(iss, token, ','); // timestamp
        candle.time = int(std::stod(token));
        std::getline(iss, token, ','); // open
        candle.open = std::stod(token);
        std::getline(iss, token, ','); // high
        candle.high = std::stod(token);
        std::getline(iss, token, ','); // low
        candle.low = std::stod(token);
        std::getline(iss, token, ','); // close
        candle.close = std::stod(token);
        std::getline(iss, token, ','); // volume
        candle.volume = std::stod(token);

        return candle;
    }

    std::vector<Candle> readCSVFile(std::string dataFileName) {
        std::ifstream file(dataFileName);
        std::vector<Candle> candles;

        std::string line;
        std::getline(file, line); // skip header
        while (std::getline(file, line)) {
            Candle candle = readCSVCandle(line);
            candles.push_back(candle);
        }
        return candles;
    }

    time_t BacktestMarket::time() const {
        if (current == -1) {
            return 0;
        }
        return candles[current].time;
    }

    bool BacktestMarket::order(Order order) {
        assert(current != -1 && current < candles.size());
        order.time = time();
        order.price = candles[current].close;

        if (order.side == OrderSide::RESET) {
            balance.assetA += balance.assetB * order.price * (1.0 - DEFAULT_FEE);
            balance.assetB = 0;
        } else if (order.side == OrderSide::BUY) {
            balance.assetB += order.amount * balance.assetA / order.price * (1.0 - DEFAULT_FEE);
            balance.assetA -= order.amount * balance.assetA;
        } else if (order.side == OrderSide::SELL) {
            balance.assetB -= order.amount * balance.assetA / order.price;
            balance.assetA += order.amount * balance.assetA * (1.0 - DEFAULT_FEE);
        }

        balance.update(order.price, order.time);
        if (saveHistory) {
            balanceHistory.back() = balance;
            saveOrder(order);
        }
        
        return true;
    }

    bool BacktestMarket::update() {
        if (current + 1 >= candles.size()) {
            return false;
        }
        ++current;

        balance.update(candles[current].close, time());
        if (saveHistory) {
            balanceHistory.push_back(balance);
        }

        maxBalance = std::max(maxBalance, balance.asAssetA());
        maxDrawdown = std::max(maxDrawdown, maxBalance - balance.asAssetA());
    
        return true;
    }

    BacktestMarket::BacktestMarket(
        const Helpers::VectorView<Candle>& candles,
        bool saveHistory
    ) : candles(candles), saveHistory(saveHistory) {
        current = -1;

        if (candles.size() > 1) {
            candleTimeDelta = candles[1].time - candles[0].time;
        } else {
            candleTimeDelta = 0;
        }

        update();
    }

    void BacktestMarket::finish() {
        current = int(candles.size()) - 1;
    }

    void BacktestMarket::restart() {
        finish();

        balance = Balance();
        if (saveHistory) {
            balanceHistory.clear();
            orderHistory.clear();
        }

        current = -1;
        update();
    }

    double BacktestMarket::getFitness() const {
        return balance.asAssetA() - maxDrawdown;
    }

    Helpers::VectorView<Candle> BacktestMarket::getCandles() const {
        if (current == candles.size()) {
            return candles;
        }
        return candles.subView(0, current + 1);
    }

} // namespace TradingBot
