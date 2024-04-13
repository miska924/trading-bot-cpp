#pragma once

#include <thread>
#include <atomic>
#include <mutex>

#include "strategies/strategy.h"
#include "features/ema_feature.h"


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
        MACDStrategy() = default;
        MACDStrategy(Market* _market, const ParamSet& paramSet);
        MACDStrategy(
            Market* market,
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD
        );
        virtual void step() override;
        virtual ParamSet getDefaultParamSet() const override;
        virtual ParamSet getMinParamSet() const override;
        virtual ParamSet getMaxParamSet() const override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;

    protected:
        EMAFeature fastEMA;
        EMAFeature slowEMA;
        EMAFeature previousFastEMA;
        EMAFeature previousSlowEMA;
    };

    class MACDHoldSlowStrategy : public MACDStrategy {
    public:
        MACDHoldSlowStrategy() = default;
        MACDHoldSlowStrategy(Market* _market, const ParamSet& parameters);
        MACDHoldSlowStrategy(
            Market* _market,
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD
        );
        virtual void step() override;
    protected:
        int hold = 1;
        int waitUntill = 0;
    };

    class MACDHoldFixedCandlesStrategy : public MACDHoldSlowStrategy {
    public:
        MACDHoldFixedCandlesStrategy() = default;
        MACDHoldFixedCandlesStrategy(Market* _market, const ParamSet& parameters);
        MACDHoldFixedCandlesStrategy(
            Market* _market,
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD,
            int holdCandles = DEFAULT_MACD_HOLD_CANDLES
        );
        virtual void step() override;
        virtual ParamSet getDefaultParamSet() const override;
        virtual ParamSet getMinParamSet() const override;
        virtual ParamSet getMaxParamSet() const override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;
    protected:
        int hold = 1;
    };

} // namespace TradingBot
