#include "markets/backtest_market.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>

#include "helpers/date_time.h"


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

    void writeCSVFile(const std::string& file, const Helpers::VectorView<Candle>& candles) {
        std::ofstream fout(file);
        fout << ",time,open,high,low,close,volume" << std::endl;

        for (size_t i = 0; i < candles.size(); ++i) {
            fout << Helpers::DateTime(candles[i].time) << ",";
            fout << candles[i].time << ",";
            fout << candles[i].open << ",";
            fout << candles[i].high << ",";
            fout << candles[i].low << ",";
            fout << candles[i].close << ",";
            fout << candles[i].volume << std::endl;
        }
    }


    time_t BacktestMarket::time() const {
        if (current == -1) {
            return 0;
        }
        return candles[current].time;
    }

    bool BacktestMarket::order(Order order) {
        if (balance.asAssetA() <= 0) {
            return false;
        }

        assert(current != -1 && current < candles.size());
        order.time = time();
        order.price = candles[current].close;

        double all = balance.asAssetA();
        double amount = order.amount * all;

        if (order.side == OrderSide::RESET) {
            if (balance.assetB > 0) {
                balance.assetA += balance.assetB * order.price * (1.0 - fee);
            } else {
                balance.assetA -= (-balance.assetB) * order.price * (1.0 + fee);
            }
            balance.assetB = 0;
        } else if (order.side == OrderSide::BUY) {
            balance.assetB += amount / order.price;
            balance.assetA -= amount * (1.0 + fee);
        } else if (order.side == OrderSide::SELL) {
            balance.assetB -= amount / order.price;
            balance.assetA += amount * (1.0 - fee);
        }

        balance.update(order.price, order.time);
        if (saveHistory) {
            balanceHistory.back() = balance;
            saveOrder(order);
        }
        lastOrder = order;
        
        return true;
    }

    bool BacktestMarket::finished() const {
        return current + 1 >= candles.size();
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
        double drawdown = ((maxBalance - balance.asAssetA()) / startBalance.asAssetA());
        maxDrawdown = std::max(maxDrawdown, drawdown);
        sumSquaredDrawdown += drawdown * drawdown;

        if (verbose) {
            if (current % 1000 == 0) {
                std::cerr << std::endl;
                std::cerr << "progress:     " << double(current * 100) / candles.size() << "%" << std::endl;
                std::cerr << "candles:      " << current << "/" << candles.size() << std::endl;
                std::cerr << "balance:      " << balance.asAssetA() << std::endl;
                std::cerr << "max drawdown: " << maxDrawdown << std::endl;
                std::cerr << "max balance:  " << maxBalance << std::endl;
            }
        }
    
        return true;
    }

    BacktestMarket::BacktestMarket(
        const Helpers::VectorView<Candle>& candles,
        bool saveHistory,
        bool verbose,
        double fee,
        Balance balance
    ) :
        candles(candles),
        saveHistory(saveHistory),
        verbose(verbose),
        fee(fee),
        startBalance(balance)
    {
        current = -1;
        this->balance = balance;

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

        balance = startBalance;
        if (saveHistory) {
            balanceHistory.clear();
            orderHistory.clear();
        }

        current = -1;
        update();
    }

    double BacktestMarket::getFitness() const {
        // return balance.asAssetA() - maxDrawdown;
        return balance.asAssetA() / startBalance.asAssetA() - (std::sqrt(sumSquaredDrawdown / (current)));
    }

    Balance BacktestMarket::getStartBalance() const {
        return startBalance;
    }

    double BacktestMarket::getFee() const {
        return fee;
    }

    Helpers::VectorView<Candle> BacktestMarket::getCandles() const {
        if (current == candles.size()) {
            return candles;
        }
        return candles.subView(0, current + 1);
    }


    const Order& BacktestMarket::getLastOrder() const {
        if (!saveHistory) {
            return lastOrder;
        }

        if (orderHistory.empty()) {
            return lastOrder;
        }
        return orderHistory.back();
    }

} // namespace TradingBot
