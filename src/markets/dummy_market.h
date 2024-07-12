#pragma once

#include "markets/market.h"


namespace TradingBot {

    class DummyMarket : public Market {
    public:
        virtual bool order(Order order) override;
        virtual bool update() override;
        virtual Helpers::VectorView<Candle> getCandles() const override;
        virtual bool finished() const override;
    private:
        std::vector<Candle> candles;
    };

} // namespace TradingBot
