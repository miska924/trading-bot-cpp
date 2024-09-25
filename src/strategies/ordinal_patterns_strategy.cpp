#include "strategies/ordinal_patterns_strategy.h"
#include <array>
#include <set>
#include <math.h>


namespace TradingBot {

    constexpr int CNT = 4;
    constexpr int PATTERNS = 1 << (CNT * 2);

    OrdinalPatternsStrategy::OrdinalPatternsStrategy(
        int fitPeriod,
        int orderPeriod
    ) : fitPeriod(fitPeriod), orderPeriod(orderPeriod) {
        paramSet = {fitPeriod, orderPeriod};
        assert(checkParamSet(paramSet));
    }

    OrdinalPatternsStrategy::OrdinalPatternsStrategy(
        const ParamSet& paramSet
    ) : OrdinalPatternsStrategy(
        std::get<int>(paramSet[0]),
        std::get<int>(paramSet[1])
    ) {}

    Signal OrdinalPatternsStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }
        bool reset = false;

        if (wait == 1) {
            reset = true;
        }

        if (wait-- > 1) {
            return {};
        }

        Helpers::VectorView<Candle> allCandles = market->getCandles();
        if (allCandles.size() < fitPeriod) {
            return {};
        }

        Helpers::VectorView<Candle> candles = allCandles.subView(allCandles.size() - fitPeriod, allCandles.size());

        std::array<double, PATTERNS> buyReturns = {0.0};
        std::array<double, PATTERNS> sellReturns = {0.0};
        std::array<int, PATTERNS> present = {0};
        for (int i = 0; i + CNT + orderPeriod < candles.size(); ++i) {
            int pattern = 0;
            for (int j = 0; j < CNT; ++j) {
                pattern = (pattern << 1) + (candles[i + j + 1].close > candles[i + j].close);
                pattern = (pattern << 1) + (
                    abs(candles[i + j + 1].close / candles[i + j].close - candles[i + j].close) > 0.01
                );
            }
            double diff = (candles[i + CNT + orderPeriod].close / candles[i + CNT].close);
            double fee = market->getFee() * 2;

            buyReturns[pattern] += diff - fee - 1.0;
            sellReturns[pattern] += 1.0 / diff - fee - 1.0;
            present[pattern]++;
        }

        int currentPattern = 0;
        for (int i = candles.size() - CNT; i < candles.size(); ++i) {
            currentPattern = (currentPattern << 1) + (candles[i].close > candles[i - 1].close);
            currentPattern = (currentPattern << 1) + (abs(candles[i].close / candles[i - 1].close - candles[i - 1].close) > 0.01);
        }

        bool valuable = true; // present[currentPattern] > fitPeriod / PATTERNS; // [TODO] make it configurable or calculate it more precisely

        if (valuable) {
            if (buyReturns[currentPattern] > 1.0) {
                wait = orderPeriod;
                return {
                    .reset = reset,
                    .order = 1
                };
            }
            if (sellReturns[currentPattern] > 1.0) {
                wait = orderPeriod;
                return {
                    .reset = reset,
                    .order = -1
                };
            }
        }

        return {
            .reset = reset
        };
    }

    bool OrdinalPatternsStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 2) {
            return false;
        }

        const int* fitPeriod = std::get_if<int>(&paramSet[0]);
        const int* orderPeriod = std::get_if<int>(&paramSet[1]);
        if (fitPeriod == nullptr) {
            return false;
        }
        if (*fitPeriod < 1) {
            return false;
        }

        if (orderPeriod == nullptr) {
            return false;
        }
        if (*orderPeriod < 1) {
            return false;
        }

        return true;
    }

} // namespace TradingBot
