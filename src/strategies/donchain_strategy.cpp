#include "strategies/donchain_strategy.h"


namespace TradingBot {

    DonchainStrategy::DonchainStrategy(Market* _market, const ParamSet& paramSet):
        minQueue([](const double& a, const double& b) {
            return std::min(a, b);
        }),
        maxQueue([](const double& a, const double& b) {
            return std::max(a, b);
        }) 
    {
        assert(checkParamSet(paramSet));

        market = _market;
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

    DonchainStrategy::DonchainStrategy(Market* _market, int period):
        DonchainStrategy(_market, ParamSet{ period }) {}


    void DonchainStrategy::step() {
        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() < period + 1) {
            return;
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

        if (waitMin && close < minimum) {
            market->order({.side = OrderSide::RESET});
            market->order({.side = OrderSide::SELL, .amount = 1});
            waitMax = true;
            waitMin = false;
        } else if (waitMax && close > maximum) {
            market->order({.side = OrderSide::RESET});
            market->order({.side = OrderSide::BUY, .amount = 1});
            waitMin = true;
            waitMax = false;
        }

        minQueue.push(close);
        maxQueue.push(close);

        minQueue.pop();
        maxQueue.pop();
    }

    DonchainLastLoserStrategy::DonchainLastLoserStrategy(Market* _market, const ParamSet& paramSet):
        DonchainStrategy(_market, paramSet) {}

    DonchainLastLoserStrategy::DonchainLastLoserStrategy(Market* _market, int period):
        DonchainLastLoserStrategy(_market, ParamSet{ period }) {}


    void DonchainLastLoserStrategy::step() {
        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() < period + 1) {
            return;
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

        if (waitMin && close < minimum) {
            if (market->getBalance().assetB != 0) {
                market->order({.side = OrderSide::RESET});
            }

            if (lastPrice < close) {
                market->order({.side = OrderSide::SELL, .amount = 1});
            }

            waitMax = true;
            waitMin = false;
            lastPrice = close;
        } else if (waitMax && close > maximum) {
            if (market->getBalance().assetB != 0) {
                market->order({.side = OrderSide::RESET});
            }

            if (lastPrice > close) {
                market->order({.side = OrderSide::BUY, .amount = 1});
            }

            waitMin = true;
            waitMax = false;
            lastPrice = close;
        }

        minQueue.push(close);
        maxQueue.push(close);

        minQueue.pop();
        maxQueue.pop();
    }

} // namespace TradingBot
