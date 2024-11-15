#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <optional>

#include "features/ema_feature.h"
#include "strategies/strategy.h"


namespace TradingBot {

    const int DEFAULT_EMACrossover_FAST_PERIOD = 15;
    const int DEFAULT_EMACrossover_SLOW_PERIOD = 30;
    const int DEFAULT_EMACrossover_HOLD_CANDLES = 30;

    const int MIN_EMACrossover_FAST_PERIOD = 1;
    const int MIN_EMACrossover_SLOW_PERIOD = 1;
    const int MIN_EMACrossover_HOLD_CANDLES = 1;

    const int MAX_EMACrossover_FAST_PERIOD = 1000;
    const int MAX_EMACrossover_SLOW_PERIOD = 1000;
    const int MAX_EMACrossover_HOLD_CANDLES = 1000;

    struct EMACrossoverParameters {
        int fastPeriod = DEFAULT_EMACrossover_FAST_PERIOD;
        int slowPeriod = DEFAULT_EMACrossover_SLOW_PERIOD;
    };

    class EMACrossoverStrategy : public Strategy {
    public:
        EMACrossoverStrategy(const ParamSet& paramSet);
        EMACrossoverStrategy(
            int fastPeriod = DEFAULT_EMACrossover_FAST_PERIOD,
            int slowPeriod = DEFAULT_EMACrossover_SLOW_PERIOD
        );
        virtual Signal step(bool newCandle) override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;

        virtual std::vector<std::vector<std::pair<time_t, double> > > getPlots() override;

    protected:
        EMAFeature fastEMA;
        EMAFeature slowEMA;
        double fast = 0;
        double slow = 0;

        int fastPeriod;
        int slowPeriod;

        int aboveCombo = 0;
        int belowCombo = 0;

        std::vector<std::pair<time_t, double> > fastPlot;
        std::vector<std::pair<time_t, double> > slowPlot;
    };

    class EMACrossoverAdvancedStrategy : public EMACrossoverStrategy {
    public:
        EMACrossoverAdvancedStrategy(const ParamSet& paramSet);
        EMACrossoverAdvancedStrategy(
            int fastPeriod = DEFAULT_EMACrossover_FAST_PERIOD,
            int slowPeriod = DEFAULT_EMACrossover_SLOW_PERIOD
        );
        virtual Signal step(bool newCandle) override;
    };

    class EMACrossoverHoldFixedStrategy : public EMACrossoverStrategy {
    public:
        EMACrossoverHoldFixedStrategy(const ParamSet& parameters);
        EMACrossoverHoldFixedStrategy(
            int fastPeriod = DEFAULT_EMACrossover_FAST_PERIOD,
            int slowPeriod = DEFAULT_EMACrossover_SLOW_PERIOD,
            int holdCandles = DEFAULT_EMACrossover_HOLD_CANDLES
        );
        virtual Signal step(bool newCandle) override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;

    protected:
        int hold = 1;
        int waitUntill = 0;
        double fast = 0;
        double slow = 0;
    };

    class EMACrossoverHoldSlowStrategy : public EMACrossoverHoldFixedStrategy {
    public:
        EMACrossoverHoldSlowStrategy(const ParamSet& parameters);
        EMACrossoverHoldSlowStrategy(
            int fastPeriod = DEFAULT_EMACrossover_FAST_PERIOD,
            int slowPeriod = DEFAULT_EMACrossover_SLOW_PERIOD
        );
        virtual bool checkParamSet(const ParamSet& paramSet) const override;
    };


} // namespace TradingBot
