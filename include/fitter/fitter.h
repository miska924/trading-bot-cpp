#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <math.h>

#include "strategies/strategy.h"
#include "markets/backtest_market.h"
#include "markets/market.h"
#include "plotting/plotting.h"


namespace TradingBot {

    const unsigned DEFAULT_ITERATIONS_LIMIT = 1000;

    template<class Strat>
    class StrategyFitter {
    private:
        const Helpers::VectorView<Candle>& candles;
        int sumL;
        int sumR;

        ParamSet bestParameters;
        Balance bestBalance = {
            .assetA = 0,
            .assetB = 0
        };
        double bestFitness = 0;

        std::vector<std::thread> threadPool;
        std::vector<std::atomic<bool>> threadStatus;
        std::mutex bestMutex;
        int numThreads;

    public:
        StrategyFitter(
            const Helpers::VectorView<Candle>& candles
        );
        ~StrategyFitter();
        void singleRun(const ParamSet& parameters, std::atomic<bool>& threadStatus);
        void fit(int iterationsLimit = DEFAULT_ITERATIONS_LIMIT);
        ParamSet getBestParameters();
        Balance getBestBalance();
        void plotBestStrategy(const std::string& fileName);
    };

    template <class Strat>
    void generateParamSets(
        const Strat& emptyStrat,
        const ParamSet& paramSetMin,
        const ParamSet& paramSetMax,
        int singleParamIterations,
        std::vector<ParamSet>& paramSets,
        ParamSet& paramSet
    ) {
        if (paramSet.size() == paramSetMin.size()) {
            if (emptyStrat.checkParamSet(paramSet)) {
                paramSets.push_back(paramSet);
            }
            return;
        }
        std::variant<int, double> min = paramSetMin[paramSet.size()];
        std::variant<int, double> max = paramSetMax[paramSet.size()];
        int* intMin = std::get_if<int>(&min);
        int* intMax = std::get_if<int>(&max);
        double* doubleMin = std::get_if<double>(&min);
        double* doubleMax = std::get_if<double>(&max);


        int intStepSize;
        double doubleStepSize;
        if (intMin != nullptr) {
            intStepSize = (
                (*intMax - *intMin) + singleParamIterations - 2
            ) / (singleParamIterations - 1);
        } else {
            doubleStepSize = (*doubleMax - *doubleMin) / (singleParamIterations - 1);
        }

        for (int i = 0; i < singleParamIterations + 1; ++i) {
            if (intMin != nullptr) {
                paramSet.push_back(std::min(*intMin + intStepSize * i, *intMax));
            } else {
                paramSet.push_back(std::min(*doubleMin + doubleStepSize * i, *doubleMax));
            }
            generateParamSets(
                emptyStrat,
                paramSetMin,
                paramSetMax,
                singleParamIterations,
                paramSets,
                paramSet
            );
            paramSet.pop_back();
        }
    }

    template<class Strat>
    StrategyFitter<Strat>::StrategyFitter(
        const Helpers::VectorView<Candle>& candles
    ) : candles(candles) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) {
            numThreads = 8;
        }
        threadStatus = std::vector<std::atomic<bool>>(numThreads);
        threadPool.reserve(numThreads);
    }

    template<class Strat>
    StrategyFitter<Strat>::~StrategyFitter() {
        for (auto& thread : threadPool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    template<class Strat>
    void StrategyFitter<Strat>::singleRun(const ParamSet& paramSet, std::atomic<bool>& threadStatus) {
        BacktestMarket market = BacktestMarket(candles, false);
        Strat strategy = Strat(
            &market,
            paramSet
        );
        strategy.run();
        double fitness = market.getFitness();

        {
            std::lock_guard<std::mutex> lock(bestMutex);
            if (bestFitness < fitness) {
                bestBalance = market.getBalance();
                bestParameters = paramSet;
                bestFitness = fitness;
            }
        }

        threadStatus = true;
    }


    template<class Strat>
    void StrategyFitter<Strat>::fit(int iterationsLimit) {
        BacktestMarket m = BacktestMarket(candles, false);
        Strat s = Strat(&m);
        int singleParamIterations = std::floor(std::pow(
            double(iterationsLimit),
            1.0 / s.getDefaultParamSet().size()
        ));
        std::vector<ParamSet> paramSets;
        ParamSet paramSet;

        generateParamSets(
            s,
            s.getMinParamSet(),
            s.getMaxParamSet(),
            singleParamIterations,
            paramSets,
            paramSet
        );

        for (const ParamSet& paramSet : paramSets) {
            if (threadPool.size() < numThreads) {
                threadPool.emplace_back(
                    &StrategyFitter::singleRun,
                    this,
                    paramSet,
                    std::ref(threadStatus[threadPool.size()])
                );
                continue;
            }

            bool started = false;
            while (!started) {
                for (size_t i = 0; i < numThreads; i++) {
                    if (!threadStatus[i]) {
                        continue;
                    }
                    if (threadPool[i].joinable()) {
                        threadPool[i].join();
                    }
                    threadStatus[i] = false;
                    threadPool[i] = std::move(std::thread(
                        &StrategyFitter::singleRun,
                        this,
                        paramSet,
                        std::ref(threadStatus[i])
                    ));
                    
                    started = true;
                    break;
                }
            }
        }

        for (auto& thread : threadPool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    template <class Strat>
    ParamSet StrategyFitter<Strat>::getBestParameters() {
        return bestParameters;
    }

    template <class Strat>
    Balance StrategyFitter<Strat>::getBestBalance() {
        return bestBalance;
    }

    template <class Strat>
    void StrategyFitter<Strat>::plotBestStrategy(const std::string& fileName) {
        BacktestMarket market = BacktestMarket(candles, true);
        Strat bestStrategy = Strat(
            &market,
            bestParameters
        );
        bestStrategy.run();

        plot(
            fileName,
            market.getCandles().toVector(),
            market.getOrderHistory(),
            market.getBalanceHistory()
        );
    }

} // namespace TradingBot
