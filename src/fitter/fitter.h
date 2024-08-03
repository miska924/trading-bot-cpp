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
        Balance startBalance;
        double fee;

        Helpers::VectorView<Candle> candles;
        double fitAroundThreshold;
        bool reliable = false;
        int repeat;

        int numThreads;
        std::vector<std::thread> threadPool;
        std::vector<bool*> threadStatus;
        std::mutex updateFitnessMutex;

        int singleParamIterations;

        ParamSet paramSetMin;
        ParamSet paramSetMax;

        Helpers::MultidimVector<double> fitnessMatrix;
        Helpers::MultidimVector<double> testFitnessMatrix;
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
            int repeats = 1,
            double fitAroundThreshold = 1,
            double fee = DEFAULT_FEE,
            Balance startBalance = Balance()
        );
        ~StrategyFitter();

        void singleRun(
            const Helpers::VectorView<Candle>& candles,
            const ParamSet& parameters,
            Helpers::MultidimVector<double>& fitnessMatrix,
            bool* threadStatus,
            int index
        );
        void perform(
            const Helpers::VectorView<Candle>& candles,
            Helpers::MultidimVector<double>& fitnessMatrix
        );
        void fit(int iterationsLimit = DEFAULT_ITERATIONS_LIMIT);
        void test(const Helpers::VectorView<Candle>& candles);
        ParamSet getBestParameters();
        double getBestBalance();
        double getBestFitness();
        bool plotBestStrategy(const std::string& fileName);
        void heatmapFitnesses(const std::string& fileName);
        void heatmapTestFitnesses(const std::string& fileName);
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
            if (*intMin == *intMax) {
                paramSet.push_back(*intMin);
                generateParamSets(
                    s,
                    paramSet,
                    index
                );
                paramSet.pop_back();
                return;
            }
            intStepSize = (
                (*intMax - *intMin) + singleParamIterations - 2
            ) / (singleParamIterations - 1);
        } else {
            if (*doubleMin == *doubleMax) {
                paramSet.push_back(*doubleMin);
                generateParamSets(
                    s,
                    paramSet,
                    index
                );
                paramSet.pop_back();
                return;
            }
            doubleStepSize = (*doubleMax - *doubleMin) / (singleParamIterations - 1);
        }

        int prevInt = 0;
        double prevDouble = 0;
        bool first = true;
        for (int i = 0; i < singleParamIterations; ++i) {
            bool ok = false;
            if (intMin != nullptr) {
                int value = std::min(*intMin + intStepSize * i, *intMax);
                if (first || value != prevInt) {
                    paramSet.push_back(value);
                    prevInt = value;
                    first = false;
                    ok = true;
                }
            } else {
                double value = std::min(*doubleMin + doubleStepSize * i, *doubleMax);
                if (first || value != prevDouble) {
                    paramSet.push_back(value);
                    prevDouble = value;
                    first = false;
                    ok = true;
                }
            }

            if (!ok) {
                continue;
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
        int repeat,
        double fitAroundThreshold,
        double fee,
        Balance startBalance
    ) : candles(candles),
        paramSetMin(paramSetMin),
        paramSetMax(paramSetMax),
        fitAroundThreshold(fitAroundThreshold),
        repeat(repeat),
        fee(fee),
        startBalance(startBalance)
    {
        numThreads = std::thread::hardware_concurrency();
        --numThreads;
        if (numThreads < 1) {
            numThreads = 1;
        }

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
    void StrategyFitter<Strat>::singleRun(
        const Helpers::VectorView<Candle>& candles,
        const ParamSet& paramSet,
        Helpers::MultidimVector<double>& fitnessMatrix,
        bool* threadStatus,
        int index
    ) {
        BacktestMarket market = BacktestMarket(candles, false, false, fee, startBalance);
        Strat strategy = Strat(
            &market,
            paramSet
        );
        strategy.run();
        double fitness = market.getFitness();

        {
            std::lock_guard<std::mutex> lock(updateFitnessMutex);
            fitnessMatrix[index] = std::min(fitness, fitnessMatrix[index]);
        }

        *threadStatus = true;
    }

    template<class Strat>
    bool StrategyFitter<Strat>::checkFitnessAround(size_t i, double threshold) const {
        if (fitnessMatrix[i] <= threshold) {
            return false;
        }
        std::vector<size_t> index = fitnessMatrix.getIndex(i);

        for (size_t j = 0; j < index.size(); ++j) {
            if (0 < index[j]) {
                --index[j];
                if (!paramSetsMatrix[index].empty() && fitnessMatrix[index] <= threshold) {
                    return false;
                }
                ++index[j];
            }
            if (index[j] + 1 < fitnessMatrix.getShape()[j]) {
                ++index[j];
                if (!paramSetsMatrix[index].empty() && fitnessMatrix[index] <= threshold) {
                    return false;
                }
                --index[j];
            }
        }
        return true;
    }

    // template<class Strat>
    // bool StrategyFitter<Strat>::checkFitnessAround(size_t i, double threshold) const {
    //     std::vector<size_t> index = fitnessMatrix.getIndex(i);

    //     double sum = 0;
    //     int count = 0;

    //     for (size_t j = 0; j < index.size(); ++j) {
    //         if (0 < index[j]) {
    //             --index[j];
    //             if (!paramSetsMatrix[index].empty()) {
    //                 sum += fitnessMatrix[index];
    //                 ++count;
    //             }
    //             ++index[j];
    //         }
    //         if (index[j] + 1 < fitnessMatrix.getShape()[j]) {
    //             ++index[j];
    //             if (!paramSetsMatrix[index].empty()) {
    //                 sum += fitnessMatrix[index];
    //                 ++count;
    //             }
    //             --index[j];
    //         }
    //     }
    //     return sum / count > threshold;
    // }

    template<class Strat>
    void StrategyFitter<Strat>::perform(
        const Helpers::VectorView<Candle>& candles,
        Helpers::MultidimVector<double>& fitnessMatrix
    ) {
        for (size_t j = 0; j < paramSetsMatrix.size(); ++j) {
            const ParamSet& paramSet = paramSetsMatrix[j];
            if (paramSet.empty()) {
                continue;
            }

            fitnessMatrix[j] = 1e18;
            for (int k = 0; k < repeat; ++k) {
                if (threadPool.size() < numThreads) {
                    threadPool.emplace_back(
                        &StrategyFitter::singleRun,
                        this,
                        std::cref(candles),
                        paramSet,
                        std::ref(fitnessMatrix),
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
                            std::cref(candles),
                            paramSet,
                            std::ref(fitnessMatrix),
                            threadStatus[i],
                            j
                        ));
                        
                        started = true;
                        break;
                    }
                }
            }
        }

        for (auto& thread : threadPool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    template<class Strat>
    void StrategyFitter<Strat>::fit(int iterationsLimit) {
        int fitParams = 0;
        for (int i = 0; i < paramSetMin.size(); ++i) {
            if (paramSetMin[i] < paramSetMax[i]) {
                fitParams++;
            }
        }

        singleParamIterations = std::floor(std::pow(
            double(iterationsLimit),
            1.0 / fitParams
        ));
        
        std::vector<size_t> index(fitParams, singleParamIterations);
        paramSetsMatrix = index;
        fitnessMatrix = {index, -1e18};
        testFitnessMatrix = {index, -1e18};

        ParamSet paramSet;

        index.clear();
        Strat s = Strat(nullptr);
        generateParamSets(
            s,
            paramSet,
            index
        );

        perform(candles, fitnessMatrix);

        for (size_t i = 0; i < paramSetsMatrix.size(); ++i) {
            if (paramSetsMatrix[i].empty()) {
                continue;
            }
            bool better = fitnessMatrix[i] > bestFitness;
            bool currentReliable = checkFitnessAround(i, fitAroundThreshold);
            
            if ((!reliable && better) || (!reliable && currentReliable) || (reliable && currentReliable && better)) {
                bestParameters = paramSetsMatrix[i];
                bestFitness = fitnessMatrix[i];
                reliable |= currentReliable;
            }
        }
        assert(!bestParameters.empty());

        BacktestMarket market = BacktestMarket(candles, false, false, fee, startBalance);
        Strat bestStrategy = Strat(
            &market,
            bestParameters
        );
        bestStrategy.run();
        bestBalance = market.getBalance().asAssetA();   
        bestFitness = market.getFitness();
    }

    template<class Strat>
    void StrategyFitter<Strat>::test(const Helpers::VectorView<Candle>& candles) {
        perform(candles, testFitnessMatrix);
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
    double StrategyFitter<Strat>::getBestFitness() {
        return bestFitness;
    }

    template <class Strat>
    bool StrategyFitter<Strat>::plotBestStrategy(const std::string& fileName) {
        if (bestParameters.empty()) {
            return false;
        }

        BacktestMarket market = BacktestMarket(candles, true, false, fee, startBalance);
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
    void StrategyFitter<Strat>::heatmapTestFitnesses(const std::string& fileName) {
        heatmap(fileName, testFitnessMatrix);
    }

    template <class Strat>
    bool StrategyFitter<Strat>::isReliable() const {
        return reliable;
    }

} // namespace TradingBot
