#pragma once

#include <time.h>
#include <vector>

#include "helpers/vector_view.h"


namespace TradingBot {

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
        Balance getBalance() const;
        virtual const Order& getLastOrder() const;
        time_t getCandleTimeDelta() const;
        void payOut(double amount);
    protected:
        void saveOrder(Order order);
        Balance balance;
        Order lastOrder = {.side = OrderSide::RESET};

        std::vector<Balance> balanceHistory;
        std::vector<Order> orderHistory;
        time_t candleTimeDelta;
    };

} // namespace TradingBot
