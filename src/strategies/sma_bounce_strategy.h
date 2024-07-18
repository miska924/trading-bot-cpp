#pragma once

#include "features/sma_feature.h"
#include "atr_feature.h"
#include "strategies/strategy.h"


namespace TradingBot {

    const int DEFAULT_SMA_PERIOD = 15;
    const int DEFAULT_SMA_GAP = 1.0;
    const int DEFAULT_SMA_HOLD_CANDLES = 5;

    class SMABounceStrategy : public Strategy {
    public:
        SMABounceStrategy() = default;
        SMABounceStrategy(
            Market* market = nullptr,
            int period = DEFAULT_SMA_PERIOD,
            double gap = DEFAULT_SMA_GAP,
            int holdCandles = DEFAULT_SMA_HOLD_CANDLES
        );
        SMABounceStrategy(
            Market* market,
            const ParamSet& paramSet
        );
        virtual void step() override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;
    private:
        int hold = 1;
        int waitUntill = 0;
        SMAFeature support;
        SMAFeature resistance;
    };

} // namespace TradingBot
