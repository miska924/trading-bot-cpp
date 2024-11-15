#include "features/sma_feature.h"

#include <iostream>
#include <math.h>


namespace TradingBot {

    SMAFeature::SMAFeature(int period) :
        period(period),
        queue([](double a, double b) -> double {
            return a + b;
        }) {}

    double sum(const Helpers::VectorView<Candle>& candles, int period) {
        int size = candles.size();
        int begin = size - period;
        double sum = 0;
        for (int i = size - 1; i >= begin; --i) {
            sum += candles[i].close;
        }
        return sum;
    }

    double SMAFeature::operator()(const Helpers::VectorView<Candle>& candles, bool incremental) {
        int size = candles.size();
        assert(period <= size);

        if (!incremental) {
            return sum(candles, period) / period;
        }

        if (!queue.size()) {
            for (int i = period; i > 0; --i) {
                queue.push(candles[size - i].close);
            }
        } else {
            queue.push(candles.back().close);
            queue.pop();
        }
        return queue.getValue() / period;
    }

    int SMAFeature::getPeriod() const {
        return period;
    }

} // namespace TradingBot
