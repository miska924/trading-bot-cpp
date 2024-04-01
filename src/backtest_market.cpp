#include "markets/backtest_market.h"

#include <fstream>
#include <sstream>
#include <random>
#include <optional>
#include <iostream>


namespace trading_bot {

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
    return candles[std::min(candles.size() - 1, currentCandle)].time;
}

bool BacktestMarket::order(Order order) {
    saveOrder(order);
    return true;
}

bool BacktestMarket::update() {
    if (currentCandle >= candles.size()) {
        return false;
    }
    currentCandle++;
    return true;
}

BacktestMarket::BacktestMarket(size_t size) {
    candles.resize(size);
    time_t time = 0;
    for (Candle& candle : candles) {
        candle.time = time++;
        candle.open = rand();
        candle.close = rand();
        candle.high = rand();
        candle.low = rand();
        candle.volume = rand();
    }
}

BacktestMarket::BacktestMarket(std::string dataFileName) {
    std::ifstream file(dataFileName);

    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        Candle candle = readCSVCandle(line);
        candles.push_back(candle);
    }
}

void BacktestMarket::finish() {
    if (candles.empty()){
        return;
    }
    currentCandle = candles.size();
}

const std::vector<Candle>* BacktestMarket::allCandles() const {
    if (candles.size() == 0) {
        return &candles;
    }
    if (currentCandle >= candles.size()) {
        return &candles;
    }
    return nullptr;
}

} // trading_bot
