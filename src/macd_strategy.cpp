#include "strategies/macd_strategy.h"

#include "markets/backtest_market.h"
#include "plotting/plotting.h"


namespace TradingBot {

    MACDStrategy::MACDStrategy(Market* market, int fastPeriod, int slowPeriod) : market(market) {
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
        previousSlowEMA = EMAFeature(slowPeriod, 1);
        previousFastEMA = EMAFeature(fastPeriod, 1);
    }

    void MACDStrategy::run() {
        do {
            const std::vector<Candle>& candles = market->getCandles();
            std::optional<double> fastOptional = fastEMA(candles);
            std::optional<double> slowOptional = slowEMA(candles);
            std::optional<double> previousFastOptional = previousFastEMA(candles);
            std::optional<double> previousSlowOptional = previousSlowEMA(candles);
            if (!fastOptional || !slowOptional || !previousFastOptional || !previousSlowOptional) {
                continue;
            }

            double fast = *fastOptional;
            double slow = *slowOptional;
            double previousFast = *previousFastOptional;
            double previousSlow = *previousSlowOptional;


            if (fast > slow && previousFast < previousSlow) {
                market->order({.side = OrderSide::RESET});
                market->order({.side = OrderSide::BUY, .amount = 1});
            } else if (fast < slow && previousFast > previousSlow) {
                market->order({.side = OrderSide::RESET});
                market->order({.side = OrderSide::SELL, .amount = 1});
            }
        } while (market->update());
    }

    MACDStrategy::MACDStrategy(Market* _market, const MACDParameters& parameters) {
        market = _market;
        slowEMA = EMAFeature(parameters.slowPeriod);
        fastEMA = EMAFeature(parameters.fastPeriod);
        previousSlowEMA = EMAFeature(parameters.slowPeriod, 1);
        previousFastEMA = EMAFeature(parameters.fastPeriod, 1);
    }

    MACD5CandlesStrategy::MACD5CandlesStrategy(Market* _market, const MACDParameters& parameters) {
        market = _market;
        slowEMA = EMAFeature(parameters.slowPeriod);
        fastEMA = EMAFeature(parameters.fastPeriod);
        previousSlowEMA = EMAFeature(parameters.slowPeriod, 1);
        previousFastEMA = EMAFeature(parameters.fastPeriod, 1);
    }

    MACD5CandlesStrategy::MACD5CandlesStrategy(Market* _market, int fastPeriod, int slowPeriod) {
        market = _market;
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
        previousSlowEMA = EMAFeature(slowPeriod, 1);
        previousFastEMA = EMAFeature(fastPeriod, 1);
    }

    void MACD5CandlesStrategy::run() {
        time_t wait5Candles = 5 * market->getCandleTimeDelta();
        time_t waitUntill = 0;
        do {
            if (market->time() < waitUntill) {
                continue;
            }
            if (market->getBalance().assetB != 0) {
                market->order({.side = OrderSide::RESET});
            }

            const std::vector<Candle>& candles = market->getCandles();

            std::optional<double> fastOptional = fastEMA(candles);
            std::optional<double> slowOptional = slowEMA(candles);
            std::optional<double> previousFastOptional = previousFastEMA(candles);
            std::optional<double> previousSlowOptional = previousSlowEMA(candles);
            if (!fastOptional || !slowOptional || !previousFastOptional || !previousSlowOptional) {
                continue;
            }

            double fast = *fastOptional;
            double slow = *slowOptional;
            double previousFast = *previousFastOptional;
            double previousSlow = *previousSlowOptional;

            if (fast > slow && previousFast < previousSlow) {
                market->order({.side = OrderSide::BUY, .amount = 1});
                waitUntill = market->time() + wait5Candles;
            } else if (fast < slow && previousFast > previousSlow) {
                market->order({.side = OrderSide::SELL, .amount = 1});
                waitUntill = market->time() + wait5Candles;
            }
        } while (market->update());
    }

    MACD5CandlesStrategyFitter::MACD5CandlesStrategyFitter(
        const std::vector<Candle>& candles,
        int sumL,
        int sumR
    ) : candles(candles), sumL(sumL), sumR(sumR) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) {
            numThreads = 8;
        }
        threadStatus = std::vector<std::atomic<bool>>(numThreads);
        threadPool.reserve(numThreads);
    }

    MACD5CandlesStrategyFitter::~MACD5CandlesStrategyFitter() {
        for (auto& thread : threadPool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void MACD5CandlesStrategyFitter::singleRun(int fastPeriod, int slowPeriod, std::atomic<bool>& threadStatus) {
        BacktestMarket market = BacktestMarket(candles);
        MACD5CandlesStrategy strategy = MACD5CandlesStrategy(
            &market,
            fastPeriod,
            slowPeriod
        );
        strategy.run();
        Balance balance = market.getBalance();

        {
            std::lock_guard<std::mutex> lock(bestMutex);
            if (bestBalance < balance) {
                bestBalance = balance;
                bestParameters = {fastPeriod, slowPeriod};
            }
        }

        threadStatus = true;
    }

    void MACD5CandlesStrategyFitter::fit() {
        for (int sum = sumL; sum < sumR; sum *= 2) {
            for (int fastPeriod = 1; fastPeriod * 2 < sum; fastPeriod *= 2) {
                int slowPeriod = sum - fastPeriod;
                
                if (threadPool.size() < numThreads) {
                    threadPool.emplace_back(
                        &MACD5CandlesStrategyFitter::singleRun,
                        this,
                        fastPeriod,
                        slowPeriod,
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
                            &MACD5CandlesStrategyFitter::singleRun,
                            this,
                            fastPeriod,
                            slowPeriod,
                            std::ref(threadStatus[i])
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

    MACDParameters MACD5CandlesStrategyFitter::getBestParameters() {
        return bestParameters;
    }

    Balance MACD5CandlesStrategyFitter::getBestBalance() {
        return bestBalance;
    }

    void MACD5CandlesStrategyFitter::plotBestStrategy() {
        BacktestMarket market = BacktestMarket(candles);
        MACD5CandlesStrategy bestStrategy = MACD5CandlesStrategy(
            &market,
            bestParameters
        );
        bestStrategy.run();

        plot(
            "TestMACD5CandlesStrategyFit.png",
            market.getCandles(),
            market.getOrderHistory(),
            market.getBalanceHistory()
        );
    }

} // namespace TradingBot
