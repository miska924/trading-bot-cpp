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

    const std::vector<Order>& Market::history() const {
        return orderHistory;
    }

    const std::vector<Candle>& Market::getCandles() const {
        return candles;
    }

    void Market::saveOrder(Order order) {
        order.time = time();
        orderHistory.push_back(order);
    }

} // namespace TradingBot
