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
        AutoFitStrategy(const ParamSet& paramSet, const ParamSet& paramSetMin, const ParamSet& paramSetMax);
        AutoFitStrategy(
            int fitWindow = DEFAULT_FIT_WINDOW,
            int testWindow = DEFAULT_TEST_WINDOW,
            int fitStep = DEFAULT_FIT_STEP,
            int fitIterations = DEFAULT_FIT_ITERATIONS,
            bool forceStop = false,
            double fitAroundThreshold = 1,
            const ParamSet& paramSetMin = {},
            const ParamSet& paramSetMax = {}
        );
        virtual Signal step(bool newCandle) override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;
        virtual std::vector<std::vector<std::pair<time_t, double> > > getPlots() override;

    protected:
        std::vector<std::vector<std::pair<time_t, double> > > plots;

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
        const ParamSet& paramSet,
        const ParamSet& paramSetMin,
        const ParamSet& paramSetMax
    ) : paramSetMin(paramSetMin), paramSetMax(paramSetMax) {
        assert(checkParamSet(paramSet));
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
    }

    template <class Strat>
    std::vector<std::vector<std::pair<time_t, double> > > AutoFitStrategy<Strat>::getPlots() {
        std::vector<std::vector<std::pair<time_t, double> > > result = plots;
        for (const auto& plot : strategy.getPlots()) {
            result.push_back(plot);
        }
        return result;
    }

    template <class Strat>
    Signal AutoFitStrategy<Strat>::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        if (market->getCandles().size() < fitWindow + testWindow) {
            return {};
        }

        bool reset = false;
        if (nextFit <= market->time() && (forceStop || market->getBalance().assetB == 0)) {
            plots = getPlots();
            if (market->getBalance().assetB != 0) {
                reset = true;
            }

            nextFit = market->time() + fitStep * market->getCandleTimeDelta();

            const Helpers::VectorView<Candle>& allCandles = market->getCandles();
            StrategyFitter<Strat> fitter(
                allCandles.subView(
                    allCandles.size() - testWindow - fitWindow,
                    allCandles.size() - testWindow
                ),
                paramSetMin,
                paramSetMax,
                1,
                fitAroundThreshold,
                market->getFee(),
                market->getStartBalance()
            );
            fitter.fit(fitIterations);

            std::cerr << fitter.getBestParameters() << " "
                      << fitter.getBestBalance() << " "
                      << fitter.getBestFitness() << "\n";

            reliable = false;
            if (fitter.isReliable()) {
                bool testPassed = false;

                if (testWindow) {
                    BacktestMarket testMarket = BacktestMarket(
                        allCandles.subView(
                            allCandles.size() - testWindow,
                            allCandles.size()
                        ),
                        false,
                        false,
                        market->getFee(),
                        market->getBalance()
                    );
                    Strat testStrategy = Strat(fitter.getBestParameters());
                    SimpleTrader(&testStrategy, &testMarket).run();
                    double fitness = testMarket.getFitness();

                    if (fitness > fitAroundThreshold) {
                        testPassed = true;
                    }
                }

                if (!testWindow || testPassed) {
                    reliable = true;
                    strategy = Strat(fitter.getBestParameters());
                    strategy.attachMarketInfo(market);
                }
            }
        }

        Signal signal = {
            .reset = reset
        };

        if (reliable) {
            Signal strategySignal = strategy.step(newCandle);
            signal.reset |= strategySignal.reset;
            signal.order = strategySignal.order;
        }

        return signal;
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
