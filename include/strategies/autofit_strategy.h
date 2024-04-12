#pragma once

#include "strategies/strategy.h"
#include "fitter/fitter.h"
#include "markets/market.h"
#include "markets/backtest_market.h"

namespace TradingBot {

    const int DEFAULT_FIT_WINDOW = 10000;
    const int DEFAULT_FIT_STEP = 1000;
    const int DEFAULT_FIT_ITERATIONS = 100;
    
    const int MIN_FIT_WINDOW = 1;
    const int MIN_FIT_STEP = 0;
    const int MIN_FIT_ITERATIONS = 1;

    const int MAX_FIT_WINDOW = 10000;
    const int MAX_FIT_STEP = 10000;
    const int MAX_FIT_ITERATIONS = 10000;

    template <class Strat>
    class AutoFitStrategy : public Strategy {
    public:
        AutoFitStrategy() = default;
        AutoFitStrategy(Market* market, const ParamSet& paramSet);
        AutoFitStrategy(
            Market* market,
            int fitWindow = DEFAULT_FIT_WINDOW,
            int fitStep = DEFAULT_FIT_STEP,
            int fitIterations = DEFAULT_FIT_ITERATIONS,
            bool forceStop = false
        );
        virtual void step() override;
        virtual ParamSet getDefaultParamSet() const override;
        virtual ParamSet getMinParamSet() const override;
        virtual ParamSet getMaxParamSet() const override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;

    protected:
        int fitWindow;
        int fitStep;
        int nextFit = 0;
        bool forceStop;
        Strat strategy;
        int fitIterations = DEFAULT_FIT_ITERATIONS;
    };

    template <class Strat>
    AutoFitStrategy<Strat>::AutoFitStrategy(Market* market, const ParamSet& paramSet) {
        this->market = market;
        this->paramSet = paramSet;
        fitWindow = std::get<int>(paramSet[0]);
        fitStep = std::get<int>(paramSet[1]);
        fitIterations = std::get<int>(paramSet[2]);
        forceStop = std::get<int>(paramSet[3]);
    }

    template <class Strat>
    AutoFitStrategy<Strat>::AutoFitStrategy(
        Market* market,
        int fitWindow,
        int fitStep,
        int fitIterations,
        bool forceStop
    ) : fitWindow(fitWindow), fitStep(fitStep), fitIterations(fitIterations), forceStop(forceStop) {
        this->market = market;
        paramSet = {
            fitWindow,
            fitStep,
            fitIterations,
            forceStop
        };
    }

    template <class Strat>
    void AutoFitStrategy<Strat>::step() {
        if (market->getCandles().size() < fitWindow) {
            return;
        }
        
        if (nextFit <= market->time() && (forceStop || market->getBalance().assetB == 0)) {
            if (forceStop) {
                market->order(Order{.side = OrderSide::RESET});
            }

            nextFit = market->time() + fitStep * market->getCandleTimeDelta();

            const Helpers::VectorView<Candle>& allCandles = market->getCandles();
            Helpers::VectorView<Candle> candles = allCandles.subView(
                allCandles.size() - fitWindow,
                allCandles.size()
            );

            StrategyFitter<Strat> fitter(candles);
            fitter.fit(fitIterations);

            ParamSet params = fitter.getBestParameters();
            strategy = Strat(market, params);
        }

        strategy.step();
    }

    template <class Strat>
    ParamSet AutoFitStrategy<Strat>::getDefaultParamSet() const {
        return {
            DEFAULT_FIT_WINDOW,
            DEFAULT_FIT_STEP,
            DEFAULT_FIT_ITERATIONS,
            0,
        };
    }

    template <class Strat>
    ParamSet AutoFitStrategy<Strat>::getMinParamSet() const {
        return {
            MIN_FIT_WINDOW,
            MIN_FIT_STEP,
            MIN_FIT_ITERATIONS,
            0,
        };
    }

    template <class Strat>
    ParamSet AutoFitStrategy<Strat>::getMaxParamSet() const {
        return {
            MAX_FIT_WINDOW,
            MAX_FIT_STEP,
            MAX_FIT_ITERATIONS,
            1,
        };
    }

    template <class Strat>
    bool AutoFitStrategy<Strat>::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 4) {
            return false;
        }

        const int* fitWindow = std::get_if<int>(&paramSet[0]);
        const int* fitStep = std::get_if<int>(&paramSet[1]);
        const int* fitIterations = std::get_if<int>(&paramSet[2]);
        const int* forceStop = std::get_if<int>(&paramSet[3]);

        if (fitWindow == nullptr || fitStep == nullptr || fitIterations == nullptr || forceStop == nullptr) {
            return false;
        }

        if (*fitWindow < 1 || *fitStep < 0 || *fitIterations < 1 || *forceStop < 0 || *forceStop > 1) {
            return false;
        }

        return true;
    }

} // namespace TradingBot
