#pragma once

#include <atomic>
#include <math.h>
#include <mutex>
#include <thread>
#include <vector>

#include "helpers/multidim_vector.h"
#include "markets/backtest_market.h"
#include "markets/market.h"
#include "plotting/plotting.h"
#include "strategies/strategy.h"


namespace TradingBot {

    const unsigned DEFAULT_ITERATIONS_LIMIT = 1000;

    template<class Strat>
    class StrategyFitter {
    private:
        Helpers::VectorView<Candle> candles;
        double aroundFitnessThreshold;
        bool reliable = false;

        int numThreads;
        std::vector<std::thread> threadPool;
        std::vector<bool*> threadStatus;
        std::mutex updateFitnessMutex;

        int singleParamIterations;

        ParamSet paramSetMin;
        ParamSet paramSetMax;

        Helpers::MultidimVector<double> fitnessMatrix;
        Helpers::MultidimVector<ParamSet> paramSetsMatrix;

        ParamSet bestParameters;
        double bestBalance = -1e18;
        double bestFitness = -1e18;

        void generateParamSets(
            const Strat& s,
            ParamSet& paramSet,
            std::vector<size_t>& index
        );

        bool checkFitnessAround(size_t i, double threshold) const;
    public:
        StrategyFitter(
            const Helpers::VectorView<Candle>& candles,
            const ParamSet& paramSetMin,
            const ParamSet& paramSetMax,
            double aroundFitnessThreshold = Balance().asAssetA()
        );
        ~StrategyFitter();
        void singleRun(const ParamSet& parameters, bool* threadStatus, int index);
        void fit(int iterationsLimit = DEFAULT_ITERATIONS_LIMIT);
        ParamSet getBestParameters();
        double getBestBalance();
        bool plotBestStrategy(const std::string& fileName);
        void heatmapFitnesses(const std::string& fileName);
        bool isReliable() const;
    };

    template <class Strat>
    void StrategyFitter<Strat>::generateParamSets(
        const Strat& s,
        ParamSet& paramSet,
        std::vector<size_t>& index
    ) {
        if (paramSet.size() == paramSetMin.size()) {
            if (s.checkParamSet(paramSet)) {
                paramSetsMatrix[index] = paramSet;
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

        for (int i = 0; i < singleParamIterations; ++i) {
            if (intMin != nullptr) {
                paramSet.push_back(std::min(*intMin + intStepSize * i, *intMax));
            } else {
                paramSet.push_back(std::min(*doubleMin + doubleStepSize * i, *doubleMax));
            }
            index.push_back(i);
            generateParamSets(
                s,
                paramSet,
                index
            );
            index.pop_back();
            paramSet.pop_back();
        }
    }

    template<class Strat>
    StrategyFitter<Strat>::StrategyFitter(
        const Helpers::VectorView<Candle>& candles,
        const ParamSet& paramSetMin,
        const ParamSet& paramSetMax,
        double aroundFitnessThreshold
    ) : candles(candles),
        paramSetMin(paramSetMin),
        paramSetMax(paramSetMax),
        aroundFitnessThreshold(aroundFitnessThreshold)
    {
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
    void StrategyFitter<Strat>::singleRun(const ParamSet& paramSet, bool* threadStatus, int index) {
        BacktestMarket market = BacktestMarket(candles, false);
        Strat strategy = Strat(
            &market,
            paramSet
        );
        strategy.run();
        double fitness = market.getFitness();

        {
            std::lock_guard<std::mutex> lock(updateFitnessMutex);
            fitnessMatrix[index] = fitness;
        }

        *threadStatus = true;
    }

    template<class Strat>
    bool StrategyFitter<Strat>::checkFitnessAround(size_t i, double threshold) const {
        std::vector<size_t> index = fitnessMatrix.getIndex(i);

        for (size_t j = 0; j < index.size(); ++j) {
            if (0 < index[j]) {
                --index[j];
                if (!paramSetsMatrix[index].empty() && fitnessMatrix[index] < threshold) {
                    return false;
                }
                ++index[j];
            }
            if (index[j] + 1 < fitnessMatrix.getShape()[j]) {
                ++index[j];
                if (!paramSetsMatrix[index].empty() && fitnessMatrix[index] < threshold) {
                    return false;
                }
                --index[j];
            }
        }
        return true;
    }

    template<class Strat>
    void StrategyFitter<Strat>::fit(int iterationsLimit) {
        singleParamIterations = std::floor(std::pow(
            double(iterationsLimit),
            1.0 / paramSetMin.size()
        ));
        
        std::vector<size_t> index(paramSetMin.size(), singleParamIterations);
        paramSetsMatrix = index;
        fitnessMatrix = {index, -1e18};

        ParamSet paramSet;

        index.clear();
        Strat s = Strat(nullptr);
        generateParamSets(
            s,
            paramSet,
            index
        );

        for (size_t j = 0; j < paramSetsMatrix.size(); ++j) {
            const ParamSet& paramSet = paramSetsMatrix[j];
            if (paramSet.empty()) {
                continue;
            }

            if (threadPool.size() < numThreads) {
                threadPool.emplace_back(
                    &StrategyFitter::singleRun,
                    this,
                    paramSet,
                    threadStatus[threadPool.size()],
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

        for (size_t i = 0; i < paramSetsMatrix.size(); ++i) {
            if (paramSetsMatrix[i].empty()) {
                continue;
            }

            bool better = fitnessMatrix[i] > bestFitness;
            bool currentReliable = checkFitnessAround(i, aroundFitnessThreshold);
            
            if ((!reliable && better) || (!reliable && currentReliable) || (reliable && currentReliable && better)) {
                bestParameters = paramSetsMatrix[i];
                bestFitness = fitnessMatrix[i];
                reliable |= currentReliable;
            }
        }
        assert(!bestParameters.empty());

        BacktestMarket market = BacktestMarket(candles, false);
        Strat bestStrategy = Strat(
            &market,
            bestParameters
        );
        bestStrategy.run();
        bestBalance = market.getBalance().asAssetA();
        
    }

    template <class Strat>
    ParamSet StrategyFitter<Strat>::getBestParameters() {
        return bestParameters;
    }

    template <class Strat>
    double StrategyFitter<Strat>::getBestBalance() {
        return bestBalance;
    }

    template <class Strat>
    bool StrategyFitter<Strat>::plotBestStrategy(const std::string& fileName) {
        if (bestParameters.empty()) {
            return false;
        }

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
        return true;
    }

    template <class Strat>
    void StrategyFitter<Strat>::heatmapFitnesses(const std::string& fileName) {
        heatmap(fileName, fitnessMatrix);
    }

    template <class Strat>
    bool StrategyFitter<Strat>::isReliable() const {
        return reliable;
    }

} // namespace TradingBot
