#include "strategies/donchain_strategy.h"


namespace TradingBot {

    DonchainStrategy::DonchainStrategy(const ParamSet& paramSet):
        minQueue([](const double& a, const double& b) {
            return std::min(a, b);
        }),
        maxQueue([](const double& a, const double& b) {
            return std::max(a, b);
        }) 
    {
        assert(checkParamSet(paramSet));
        period = std::get<int>(paramSet[0]);
    }

    bool DonchainStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 1) {
            return false;
        }
        if (std::get_if<int>(&paramSet[0]) == nullptr) {
            return false;
        }
        if (*std::get_if<int>(&paramSet[0]) < 1) {
            return false;
        }
        return true;
    }

    std::vector<std::vector<std::pair<time_t, double> > > DonchainStrategy::getPlots() {
        return { minPlot, maxPlot };
    }

    DonchainStrategy::DonchainStrategy(int period):
        DonchainStrategy(ParamSet{ period }) {}

    Signal DonchainStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() < period + 1) {
            return {};
        }

        while (minQueue.size() < period) {
            double value = candles[candles.size() - period - 1 + minQueue.size()].close;
            minQueue.push(value);
            maxQueue.push(value);
        }

        Candle lastCandle = candles.back();
        double close = lastCandle.close;

        double minimum = minQueue.functionValue();
        double maximum = maxQueue.functionValue();

        minPlot.push_back({lastCandle.time, minimum});
        maxPlot.push_back({lastCandle.time, maximum});

        bool reset = false;
        double order = 0.0;

        if (waitMin && close < minimum) {
            reset = true;
            order = -1;
            waitMax = true;
            waitMin = false;
        } else if (waitMax && close > maximum) {
            reset = true;
            order = 1;
            waitMin = true;
            waitMax = false;
        }

        minQueue.push(close);
        maxQueue.push(close);

        minQueue.pop();
        maxQueue.pop();

        return {
            .reset = reset,
            .order = order,
        };
    }

    DonchainLastLoserStrategy::DonchainLastLoserStrategy(const ParamSet& paramSet):
        DonchainStrategy(paramSet) {}

    DonchainLastLoserStrategy::DonchainLastLoserStrategy(int period):
        DonchainLastLoserStrategy(ParamSet{ period }) {}


    Signal DonchainLastLoserStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() < period + 1) {
            return {};
        }

        while (minQueue.size() < period) {
            double value = candles[candles.size() - period - 1 + minQueue.size()].close;
            minQueue.push(value);
            maxQueue.push(value);
        }

        Candle lastCandle = candles.back();
        double close = lastCandle.close;

        double minimum = minQueue.functionValue();
        double maximum = maxQueue.functionValue();

        minPlot.push_back({lastCandle.time, minimum});
        maxPlot.push_back({lastCandle.time, maximum});

        bool reset = false;
        double order = 0.0;
        if (waitMin && close < minimum) {
            if (market->getBalance().assetB != 0) {
                reset = true;
            }

            if (lastPrice < close) {
                order = -1;
            }

            waitMax = true;
            waitMin = false;
            lastPrice = close;
        } else if (waitMax && close > maximum) {
            if (market->getBalance().assetB != 0) {
                reset = true;
            }

            if (lastPrice > close) {
                order = 1;
            }

            waitMin = true;
            waitMax = false;
            lastPrice = close;
        }

        minQueue.push(close);
        maxQueue.push(close);

        minQueue.pop();
        maxQueue.pop();

        return {
            .reset = reset,
            .order = order,
        };
    }

} // namespace TradingBot
