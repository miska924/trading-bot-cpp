#pragma once

#include "strategies/strategy.h"
#include "features/ema_feature.h"


namespace TradingBot {

    const double DEFAULT_MACD_FAST_PERIOD = 15;
    const double DEFAULT_MACD_SLOW_PERIOD = 30;

    class MACDStrategy : public Strategy {
    public:
        MACDStrategy(
            Market* market,
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD
        );
        virtual void run() override;
    protected:
        Market* market;
        EMAFeature fastEMA;
        EMAFeature slowEMA;
        EMAFeature previousFastEMA;
        EMAFeature previousSlowEMA;
    };

} // namespace TradingBot
