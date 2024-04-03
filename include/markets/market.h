#pragma once

#include <time.h>
#include <vector>


namespace TradingBot {

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
        BUY,
        SELL
    };

    struct Order {
        time_t time;
        OrderSide side;
        double amount;

        bool operator==(const Order& other) const;
    };

    class Market {
    public:
        virtual ~Market() = default;
        virtual time_t time() const;
        virtual bool order(Order order) = 0;
        virtual bool update() = 0;
        const std::vector<Order>& history() const;
        const std::vector<Candle>& getCandles() const;
    protected:
        void saveOrder(Order order);
        std::vector<Candle> candles;
    private:
        std::vector<Order> orderHistory;
    };

} // TradingBot
