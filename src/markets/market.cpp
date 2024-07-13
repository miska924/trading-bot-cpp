#include "markets/market.h"

#include <chrono>
#include <time.h>


namespace TradingBot {

    bool Candle::operator==(const Candle& other) const {
        return time == other.time
            && open == other.open
            && high == other.high
            && low == other.low
            && close == other.close
            && volume == other.volume;
    }

    bool Order::operator==(const Order& other) const {
        return side == other.side && amount == other.amount;
    }

    time_t Market::time() const {
        return std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
        );
    }

    const std::vector<Order>& Market::getOrderHistory() const {
        return orderHistory;
    }

    const std::vector<Balance>& Market::getBalanceHistory() const {
        return balanceHistory;
    }

    bool Market::finished() const {
        return false;
    }

    Balance Market::getBalance() const {
        return balance;
    }

    double Market::getFee() const {
        return fee;
    }

    Balance Market::getStartBalance() const {
        return Balance();
    }

    const Order& Market::getLastOrder() const {
        if (orderHistory.empty()) {
            return lastOrder;
        }
        return orderHistory.back();
    }

    time_t Market::getCandleTimeDelta() const {
        return candleTimeDelta;
    }

    void Market::saveOrder(Order order) {
        orderHistory.push_back(order);
    }

    double Balance::asAssetA() const {
        return assetA + assetB * price;
    }

    void Balance::update(double newPrice, time_t newTime) {
        price = newPrice;
        time = newTime;
    }

    bool Balance::operator<(const Balance& other) const {
        return asAssetA() < other.asAssetA();
    }

} // namespace TradingBot
