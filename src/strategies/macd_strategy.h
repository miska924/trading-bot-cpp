#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <optional>

#include "features/ema_feature.h"
#include "strategies/strategy.h"


namespace TradingBot {

    const int DEFAULT_MACD_FAST_PERIOD = 15;
    const int DEFAULT_MACD_SLOW_PERIOD = 30;
    const int DEFAULT_MACD_HOLD_CANDLES = 30;

    const int MIN_MACD_FAST_PERIOD = 1;
    const int MIN_MACD_SLOW_PERIOD = 1;
    const int MIN_MACD_HOLD_CANDLES = 1;

    const int MAX_MACD_FAST_PERIOD = 1000;
    const int MAX_MACD_SLOW_PERIOD = 1000;
    const int MAX_MACD_HOLD_CANDLES = 1000;

    struct MACDParameters {
        int fastPeriod = DEFAULT_MACD_FAST_PERIOD;
        int slowPeriod = DEFAULT_MACD_SLOW_PERIOD;
    };

    class MACDStrategy : public Strategy {
    public:
        MACDStrategy(const ParamSet& paramSet);
        MACDStrategy(
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD
        );
        virtual Signal step(bool newCandle) override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;

        virtual std::vector<std::vector<std::pair<time_t, double> > > getPlots() override;

    protected:
        EMAFeature fastEMA;
        EMAFeature slowEMA;
        double fast = 0;
        double slow = 0;

        std::vector<std::pair<time_t, double> > fastPlot;
        std::vector<std::pair<time_t, double> > slowPlot;
    };

    class MACDHoldFixedStrategy : public MACDStrategy {
    public:
        MACDHoldFixedStrategy(const ParamSet& parameters);
        MACDHoldFixedStrategy(
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD,
            int holdCandles = DEFAULT_MACD_HOLD_CANDLES
        );
        virtual Signal step(bool newCandle) override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;

    protected:
        int hold = 1;
        int waitUntill = 0;
        double fast = 0;
        double slow = 0;
    };

    class MACDHoldSlowStrategy : public MACDHoldFixedStrategy {
    public:
        MACDHoldSlowStrategy(const ParamSet& parameters);
        MACDHoldSlowStrategy(
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD
        );
        virtual bool checkParamSet(const ParamSet& paramSet) const override;
    };


} // namespace TradingBot
