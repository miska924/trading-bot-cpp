#pragma once

#include "fitter/fitter.h"
#include "markets/backtest_market.h"
#include "markets/market.h"
#include "strategies/strategy.h"


namespace TradingBot {

    const int DEFAULT_FIT_WINDOW = 10000;
    const int DEFAULT_FIT_STEP = 1000;
    const int DEFAULT_FIT_ITERATIONS = 100;
    const int DEFAULT_TEST_WINDOW = 0;
    
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
        AutoFitStrategy(Market* market, const ParamSet& paramSet, const ParamSet& paramSetMin, const ParamSet& paramSetMax);
        AutoFitStrategy(
            Market* market,
            int fitWindow = DEFAULT_FIT_WINDOW,
            int testWindow = DEFAULT_TEST_WINDOW,
            int fitStep = DEFAULT_FIT_STEP,
            int fitIterations = DEFAULT_FIT_ITERATIONS,
            bool forceStop = false,
            double fitAroundThreshold = 1,
            const ParamSet& paramSetMin = {},
            const ParamSet& paramSetMax = {}
        );
        virtual void step() override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;

    protected:
        int fitWindow;
        int testWindow;
        int fitStep;
        int nextFit = 0;
        bool forceStop;
        double fitAroundThreshold;

        ParamSet paramSetMin;
        ParamSet paramSetMax;

        bool reliable = false;
        Strat strategy;
        int fitIterations = DEFAULT_FIT_ITERATIONS;
    };

    template <class Strat>
    AutoFitStrategy<Strat>::AutoFitStrategy(
        Market* market,
        const ParamSet& paramSet,
        const ParamSet& paramSetMin,
        const ParamSet& paramSetMax
    ) : paramSetMin(paramSetMin), paramSetMax(paramSetMax) {
        assert(checkParamSet(paramSet));

        this->market = market;
        this->paramSet = paramSet;
        fitWindow = std::get<int>(paramSet[0]);
        testWindow = std::get<int>(paramSet[1]);
        fitStep = std::get<int>(paramSet[2]);
        fitIterations = std::get<int>(paramSet[3]);
        forceStop = std::get<int>(paramSet[4]);
        fitAroundThreshold = std::get<double>(paramSet[5]);
    }

    template <class Strat>
    AutoFitStrategy<Strat>::AutoFitStrategy(
        Market* market,
        int fitWindow,
        int testWindow,
        int fitStep,
        int fitIterations,
        bool forceStop,
        double fitAroundThreshold,
        const ParamSet& paramSetMin,
        const ParamSet& paramSetMax
    ) :
        fitWindow(fitWindow),
        testWindow(testWindow),
        fitStep(fitStep),
        fitIterations(fitIterations),
        forceStop(forceStop),
        fitAroundThreshold(fitAroundThreshold),
        paramSetMin(paramSetMin),
        paramSetMax(paramSetMax)
    {
        paramSet = {
            fitWindow,
            testWindow,
            fitStep,
            fitIterations,
            forceStop,
            fitAroundThreshold,
        };
        assert(checkParamSet(paramSet));

        this->market = market;
    }

    template <class Strat>
    void AutoFitStrategy<Strat>::step() {
        if (market->getCandles().size() < fitWindow + testWindow) {
            return;
        }
        
        if (nextFit <= market->time() && (forceStop || market->getBalance().assetB == 0)) {
            if (market->getBalance().assetB != 0) {
                market->order(Order{.side = OrderSide::RESET});
            }

            nextFit = market->time() + fitStep * market->getCandleTimeDelta();

            const Helpers::VectorView<Candle>& allCandles = market->getCandles();
            StrategyFitter<Strat> fitter(
                allCandles.subView(
                    allCandles.size() - testWindow - fitWindow,
                    allCandles.size() - testWindow
                ),
                market->getCandleTimeDelta(),
                paramSetMin,
                paramSetMax,
                fitAroundThreshold
            );
            fitter.fit(fitIterations);

            reliable = false;
            if (fitter.isReliable()) {
                if (!testWindow) {
                    reliable = true;
                    strategy = Strat(market, fitter.getBestParameters());
                } else {
                    BacktestMarket testMarket = BacktestMarket(
                        allCandles.subView(
                            allCandles.size() - testWindow,
                            allCandles.size()
                        ),
                        TEST_CANDLES_TIMEDELTA,
                        false
                    );
                    Strat testStrategy = Strat(
                        &testMarket,
                        fitter.getBestParameters()
                    );
                    testStrategy.run();
                    double fitness = testMarket.getFitness();

                    if (fitness > fitAroundThreshold) {
                        reliable = true;
                        strategy = Strat(market, fitter.getBestParameters());
                    }
                }
            }
        }

        if (reliable) {
            strategy.step();
        }
    }

    template <class Strat>
    bool AutoFitStrategy<Strat>::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 6) {
            return false;
        }

        const int* fitWindow = std::get_if<int>(&paramSet[0]);
        const int* testWindow = std::get_if<int>(&paramSet[1]);
        const int* fitStep = std::get_if<int>(&paramSet[2]);
        const int* fitIterations = std::get_if<int>(&paramSet[3]);
        const int* forceStop = std::get_if<int>(&paramSet[4]);
        const double* fitAroundThreshold = std::get_if<double>(&paramSet[5]);
        if (fitAroundThreshold == nullptr) {
            return false;
        }

        if (testWindow == nullptr || fitWindow == nullptr || fitStep == nullptr || fitIterations == nullptr || forceStop == nullptr) {
            return false;
        }

        if (*fitWindow < 1 || *fitStep < 0 || *fitIterations < 1 || *forceStop < 0 || *forceStop > 1) {
            return false;
        }

        return true;
    }

} // namespace TradingBot
