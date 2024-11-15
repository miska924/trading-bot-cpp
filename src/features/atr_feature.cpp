#include "features/atr_feature.h"

#include <math.h>


namespace TradingBot {

    double logIf(double value, bool logPrice) {
        return logPrice ? log1p(value) : value;
    }

    double trueRange(const Helpers::VectorView<Candle>& candles, int index, int period, bool logPrice) {
        double result = logIf(candles[index].high, logPrice) - logIf(candles[index].low, logPrice);
        if (candles.size() - index < period) {
            result = std::max(
                result,
                std::max(
                    std::abs(logIf(candles[index].high, logPrice) - logIf(candles[index - 1].close, logPrice)),
                    std::abs(logIf(candles[index - 1].close, logPrice) - logIf(candles[index].low, logPrice))
                )
            );
        }
        return result;
    }

    double atr(const Helpers::VectorView<Candle>& candles, int period, bool logPrice) {
        int size = candles.size();
        int begin = size - period;
        double sum = 0;
        for (int i = size - 1; i >= begin; --i) {
            sum += trueRange(candles, i, period, logPrice);
        }
        return sum / period;
    }

    ATRFeature::ATRFeature(int period, bool logPrice) :
        period(period),
        logPrice(logPrice),
        queue([](double a, double b) -> double { return a + b; }) {}

    double ATRFeature::operator()(const Helpers::VectorView<Candle>& candles, bool incremental) {
        int size = candles.size();
        assert(period <= size);

        if (!incremental) {
            return atr(candles, period, logPrice);
        }

        if (!queue.size()) {
            for (int i = period; i > 0; --i) {
                queue.push(trueRange(candles, size - i, period, logPrice));
            }
        } else {
            queue.push(trueRange(candles, size - 1, period, logPrice));
            queue.pop();
        }

        return queue.getValue() / period;
    }

    int ATRFeature::getPeriod() const {
        return period;
    }

} // namespace TradingBot
