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
        double bestFitness = -1e18;

        std::vector<std::thread> threadPool;
        std::vector<bool*> threadStatus;
        std::mutex bestMutex;
        int bestIndex = -1;
        int numThreads;

        bool saveFitnesses;
        std::vector<double> fitnesses;
        std::vector<ParamSet> paramSets;
    public:
        StrategyFitter(
            const Helpers::VectorView<Candle>& candles,
            bool saveFitnesses = false
        );
        ~StrategyFitter();
        void singleRun(const ParamSet& parameters, bool* threadStatus, std::vector<double>& fitnesses, int index);
        void fit(int iterationsLimit = DEFAULT_ITERATIONS_LIMIT);
        ParamSet getBestParameters();
        Balance getBestBalance();
        void plotBestStrategy(const std::string& fileName);
        void heatmapFitnesses(const std::string& fileName);
        bool isReliable() const;
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
        const Helpers::VectorView<Candle>& candles,
        bool saveFitnesses
    ) : candles(candles), saveFitnesses(saveFitnesses) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads <= 1) {
            numThreads = 2;
        }
        --numThreads;

        threadStatus.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i) {
            threadStatus.push_back(new bool(false));
        }

        threadPool.reserve(numThreads);
    }

    template<class Strat>
    StrategyFitter<Strat>::~StrategyFitter() {
        for (auto& thread : threadPool) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        for (auto& status : threadStatus) {
            delete status;
        }
    }

    template<class Strat>
    void StrategyFitter<Strat>::singleRun(const ParamSet& paramSet, bool* threadStatus, std::vector<double>& fitnesses, int index) {
        BacktestMarket market = BacktestMarket(candles, false);
        Strat strategy = Strat(
            &market,
            paramSet
        );
        strategy.run();
        double fitness = market.getFitness();

        {
            std::lock_guard<std::mutex> lock(bestMutex);
            if (saveFitnesses) {
                fitnesses.push_back(fitness);
            }
            if (bestFitness < fitness || (bestFitness == fitness && index < bestIndex)) {
                bestIndex = index;
                bestBalance = market.getBalance();
                bestParameters = paramSet;
                bestFitness = fitness;
            }
        }

        *threadStatus = true;
    }


    template<class Strat>
    void StrategyFitter<Strat>::fit(int iterationsLimit) {
        BacktestMarket m = BacktestMarket(candles, false);
        Strat s = Strat(&m);
        int singleParamIterations = std::floor(std::pow(
            double(iterationsLimit),
            1.0 / s.getDefaultParamSet().size()
        ));
        
        paramSets.clear();
        paramSets.reserve(singleParamIterations * s.getDefaultParamSet().size());

        if (saveFitnesses) {
            fitnesses.clear();
            fitnesses.reserve(singleParamIterations * s.getDefaultParamSet().size());
        }

        ParamSet paramSet;

        generateParamSets(
            s,
            s.getMinParamSet(),
            s.getMaxParamSet(),
            singleParamIterations,
            paramSets,
            paramSet
        );

        for (size_t j = 0; j < paramSets.size(); ++j) {
            const ParamSet& paramSet = paramSets[j];
            if (threadPool.size() < numThreads) {
                threadPool.emplace_back(
                    &StrategyFitter::singleRun,
                    this,
                    paramSet,
                    threadStatus[threadPool.size()],
                    std::ref(fitnesses),
                    j
                );
                continue;
            }

            bool started = false;
            while (!started) {
                for (size_t i = 0; i < numThreads; i++) {
                    if (!*threadStatus[i]) {
                        continue;
                    }
                    if (threadPool[i].joinable()) {
                        threadPool[i].join();
                    }
                    *threadStatus[i] = false;
                    threadPool[i] = std::move(std::thread(
                        &StrategyFitter::singleRun,
                        this,
                        paramSet,
                        threadStatus[i],
                        std::ref(fitnesses),
                        j
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

    template <class Strat>
    void StrategyFitter<Strat>::heatmapFitnesses(const std::string& fileName) {
        heatmap(fileName, paramSets, fitnesses);
    }

    template <class Strat>
    bool StrategyFitter<Strat>::isReliable() const {
        return bestFitness > Balance().asAssetA();
    }

} // namespace TradingBot
