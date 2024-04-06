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
        return std::move(candles);
    }

    time_t BacktestMarket::time() const {
        if (candles.empty()) {
            return 0;
        }
        return candles.back().time;
    }

    bool BacktestMarket::order(Order order) {
        order.time = time();
        order.price = candles.back().close;

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
        balanceHistory.back() = balance;
        
        saveOrder(order);
        return true;
    }

    bool BacktestMarket::update() {
        if (futureCandles.empty()) {
            return false;
        }
        candles.push_back(futureCandles.back());
        futureCandles.pop_back();

        balance.update(candles.back().close, time());
        balanceHistory.push_back(balance);
    
        return true;
    }

    BacktestMarket::BacktestMarket(size_t size) {
        futureCandles.resize(size);
        time_t time = size;
        // fill in reverse order
        for (Candle& candle : futureCandles) {
            candle.time = --time;
            candle.open = rand();
            candle.close = rand();
            candle.high = rand();
            candle.low = rand();
            candle.volume = rand();
        }
        initFutureCandles = futureCandles;
        update();
        candleTimeDelta = 1;
    }

    BacktestMarket::BacktestMarket(std::string dataFileName) {
        futureCandles = readCSVFile(dataFileName);
        std::reverse(futureCandles.begin(), futureCandles.end());
        if (futureCandles.size() >= 2) {
            candleTimeDelta = futureCandles[0].time - futureCandles[1].time;
        }
        initFutureCandles = futureCandles;
        update();
    }

    BacktestMarket::BacktestMarket(const std::vector<Candle>& candles) {
        futureCandles = candles;
        std::reverse(futureCandles.begin(), futureCandles.end());
        if (futureCandles.size() >= 2) {
            candleTimeDelta = futureCandles[0].time - futureCandles[1].time;
        }
        initFutureCandles = futureCandles;
        update();
    }

    void BacktestMarket::finish() {
        if (futureCandles.empty()){
            return;
        }
        candles.insert(candles.end(), futureCandles.rbegin(), futureCandles.rend());
        futureCandles.clear();
    }

    void BacktestMarket::restart() {
        finish();

        balance = Balance();
        balanceHistory.clear();

        orderHistory.clear();

        futureCandles = initFutureCandles;
        candles.clear();
        
        update();
    }

} // namespace TradingBot
