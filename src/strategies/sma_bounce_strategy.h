#pragma once

#include <vector>

#include "features/sma_feature.h"
#include "features/atr_feature.h"
#include "strategies/strategy.h"
#include "markets/market.h"


namespace TradingBot {

    const int DEFAULT_SMA_GAP = 1.0;

    class SMABounceStrategy : public Strategy {
    public:
        SMABounceStrategy(
            Market* market = nullptr,
            int period = DEFAULT_SMA_PERIOD,
            int atrPeriod = 1000,
            double gapOutCoeff = 10,
            double gap = DEFAULT_SMA_GAP
        );
        SMABounceStrategy(
            Market* market,
            const ParamSet& paramSet
        );
        virtual void step() override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;

        virtual std::vector<std::vector<std::pair<time_t, double> > > getPlots() override;
    private:
        int period;
        int atrPeriod;
        double gap;
        double gapOutCoeff;
        bool waiting = false;
        OrderSide side = OrderSide::RESET;
        bool inGap = false;

        std::vector<std::pair<time_t, double> > ups, downs;

        SMAFeature smaFeature;
        ATRFeature atrFeature;
    };

} // namespace TradingBot
