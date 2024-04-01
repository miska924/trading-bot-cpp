#pragma once

#include <time.h>
#include <vector>


namespace trading_bot {

    struct Candle {
        time_t time;
        double open;
        double high;
        double low;
        double close;
        double volume;

        bool operator==(const Candle& other) const {
            return time == other.time
                && open == other.open
                && high == other.high
                && low == other.low
                && close == other.close
                && volume == other.volume;
        }
    };

    enum OrderSide {
        BUY,
        SELL
    };

    struct Order {
        OrderSide side;
        double amount;

        bool operator==(const Order& other) const {
            return side == other.side && amount == other.amount;
        }
    };

    class Market {
    public:
        virtual ~Market() = default;
        virtual time_t time() const;
        virtual bool order(Order order) = 0;
        virtual bool update() = 0;
        const std::vector<Order>& history() const;
    protected:
        void saveOrder(Order order);
    private:
        std::vector<Order> orderHistory;
    };

    class DummyMarket : public Market {
    public:
        virtual bool order(Order order) override;
        virtual bool update() override;
    };

} // trading_bot
