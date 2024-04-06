#pragma once

#include <thread>
#include <atomic>
#include <mutex>

#include "strategies/strategy.h"
#include "features/ema_feature.h"


namespace TradingBot {

    const double DEFAULT_MACD_FAST_PERIOD = 15;
    const double DEFAULT_MACD_SLOW_PERIOD = 30;

    struct MACDParameters {
        int fastPeriod = DEFAULT_MACD_FAST_PERIOD;
        int slowPeriod = DEFAULT_MACD_SLOW_PERIOD;
    };

    class MACDStrategy : public Strategy {
    public:
        MACDStrategy() = default;
        MACDStrategy(Market* _market, const MACDParameters& parameters);
        MACDStrategy(
            Market* market,
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD
        );
        virtual void run() override;
    protected:
        Market* market = nullptr;
        EMAFeature fastEMA;
        EMAFeature slowEMA;
        EMAFeature previousFastEMA;
        EMAFeature previousSlowEMA;
    };

    class MACD5CandlesStrategy : public MACDStrategy {
    public:
        MACD5CandlesStrategy() = default;
        MACD5CandlesStrategy(Market* _market, const MACDParameters& parameters);
        MACD5CandlesStrategy(
            Market* _market,
            int fastPeriod = DEFAULT_MACD_FAST_PERIOD,
            int slowPeriod = DEFAULT_MACD_SLOW_PERIOD
        );
        virtual void run() override;
    };

    class MACD5CandlesStrategyFitter {
    private:
        const std::vector<Candle>& candles;
        int sumL;
        int sumR;

        MACDParameters bestParameters;
        Balance bestBalance = {
            .assetA = 0,
            .assetB = 0
        };

        std::vector<std::thread> threadPool;
        std::vector<std::atomic<bool>> threadStatus;
        std::mutex bestMutex;
        int numThreads;

    public:
        MACD5CandlesStrategyFitter(
            const std::vector<Candle>& candles,
            int sumL,
            int sumR
        );
        ~MACD5CandlesStrategyFitter();
        void singleRun(int fastPeriod, int slowPeriod, std::atomic<bool>& threadStatus);
        void fit();
        MACDParameters getBestParameters();
        Balance getBestBalance();
        void plotBestStrategy();
    };


} // namespace TradingBot
