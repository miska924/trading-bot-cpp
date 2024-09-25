#pragma once

#include "traders/trader.h"

#include "features/atr_feature.h"


namespace TradingBot {

    class SLTPTrader : public Trader {
    public:
        SLTPTrader(Strategy* strategy, Market* market, double stopLoss, double takeProfit, double risk);
        virtual ~SLTPTrader() override = default;

        virtual void step(bool newCandle) override;

    private:
        double stopLoss;
        double takeProfit;
        double risk;

        ATRFeature atrFeature;

        Order setSLTP(Order order);
    };

} // namespace TradingBot
