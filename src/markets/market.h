#pragma once

#include <time.h>
#include <vector>

#include "helpers/vector_view.h"


namespace TradingBot {

    enum CandleTimeDelta {
        CANDLE_1_MIN = 60,
        CANDLE_2_MIN = 2 * CANDLE_1_MIN,
        CANDLE_3_MIN = 3 * CANDLE_1_MIN,
        CANDLE_5_MIN = 5 * CANDLE_1_MIN,
        CANDLE_10_MIN = 10 * CANDLE_1_MIN,
        CANDLE_15_MIN = 15 * CANDLE_1_MIN,
        CANDLE_30_MIN = 30 * CANDLE_1_MIN,
        CANDLE_1_HOUR = 60 * CANDLE_1_MIN,
        CANDLE_2_HOUR = 2 * CANDLE_1_HOUR,
        CANDLE_4_HOUR = 4 * CANDLE_1_HOUR,
        CANDLE_1_DAY = 24 * CANDLE_1_MIN,
        CANDLE_1_WEEK = 7 * CANDLE_1_DAY,
        CANDLE_1_MONTH = 30 * CANDLE_1_DAY,
    };

    const double DEFAULT_ASSET_A_BALANCE = 100.0;

    struct Candle {
        time_t time;
        double open;
        double high;
        double low;
        double close;
        double volume;

        bool operator==(const Candle& other) const;
    };

    enum OrderSide {
        BUY = 1,
        RESET = 0,
        SELL = -1,
    };

    struct Balance {
        double assetA = DEFAULT_ASSET_A_BALANCE;
        double assetB = 0;
        double price = 0;
        time_t time = 0;

        double asAssetA() const;
        void update(double newPrice, time_t newTime);

        bool operator<(const Balance& other) const;
    };

    struct Order {
        time_t time;
        OrderSide side;
        double amount;
        double price;
        double stopLoss = 0;
        double takeProfit = 0;

        bool operator==(const Order& other) const;
    };

    class Market {
    public:
        virtual ~Market() = default;
        virtual time_t time() const;
        virtual bool order(Order order) = 0;
        virtual bool update() = 0;
        virtual Helpers::VectorView<Candle> getCandles() const = 0;
        const std::vector<Order>& getOrderHistory() const;
        const std::vector<Balance>& getBalanceHistory() const;
        virtual Balance getBalance() const;
        virtual double getFee() const;
        virtual Balance getStartBalance() const;
        virtual const Order& getLastOrder() const;
        time_t getCandleTimeDelta() const;
        virtual bool finished() const;
    protected:
        void saveOrder(Order order);
        Balance balance;
        double fee = 0;
        Order lastOrder = {.side = OrderSide::RESET};

        std::vector<Balance> balanceHistory;
        std::vector<Order> orderHistory;
        time_t candleTimeDelta;
    };

    class MarketInfo {
    public:
        MarketInfo(Market* market);
        time_t time() const;
        Helpers::VectorView<Candle> getCandles() const;
        const std::vector<Order>& getOrderHistory() const;
        const std::vector<Balance>& getBalanceHistory() const;
        Balance getBalance() const;
        double getFee() const;
        Balance getStartBalance() const;
        const Order& getLastOrder() const;
        time_t getCandleTimeDelta() const;
        bool finished() const;
    private:
        Market* market;
    };

} // namespace TradingBot
