#pragma once

#include "traders/trader.h"

#include "features/atr_feature.h"


namespace TradingBot {

    class PercentSLTrader : public Trader {
    public:
        PercentSLTrader(Strategy* strategy, Market* market, double stopLoss, double risk);
        virtual ~PercentSLTrader() override = default;

        virtual void step(bool newCandle) override;

    private:
        double stopLoss;
        double risk;

        ATRFeature atrFeature;

        Order setSL(Order order);
    };

    class DynamicPercentSLTrader : public Trader {
    public:
        DynamicPercentSLTrader(Strategy* strategy, Market* market, double stopLoss, double risk);
        virtual ~DynamicPercentSLTrader() override = default;

        virtual void step(bool newCandle) override;

    private:
        double stopLoss;
        double risk;

        ATRFeature atrFeature;

        Order setSL(Order order);
    };

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

    class PercentSLTPTrader : public Trader {
    public:
        PercentSLTPTrader(Strategy* strategy, Market* market, double stopLoss, double takeProfit, double risk);
        virtual ~PercentSLTPTrader() override = default;

        virtual void step(bool newCandle) override;

    private:
        double stopLoss;
        double takeProfit;
        double risk;

        ATRFeature atrFeature;

        Order setSLTP(Order order);
    };

} // namespace TradingBot
