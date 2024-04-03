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

    time_t BacktestMarket::time() const {
        if (candles.empty()) {
            return 0;
        }
        return candles.back().time;
    }

    bool BacktestMarket::order(Order order) {
        order.price = candles.back().close;
        saveOrder(order);
        return true;
    }

    bool BacktestMarket::update() {
        if (futureCandles.empty()) {
            return false;
        }
        candles.push_back(futureCandles.back());
        futureCandles.pop_back();
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
        update();
    }

    BacktestMarket::BacktestMarket(std::string dataFileName) {
        std::ifstream file(dataFileName);

        std::string line;
        std::getline(file, line); // skip header
        while (std::getline(file, line)) {
            Candle candle = readCSVCandle(line);
            futureCandles.push_back(candle);
        }
        std::reverse(futureCandles.begin(), futureCandles.end());
        update();
    }

    void BacktestMarket::finish() {
        if (futureCandles.empty()){
            return;
        }
        candles.insert(candles.end(), futureCandles.rbegin(), futureCandles.rend());
        futureCandles.clear();
    }

} // namespace TradingBot
