#include "features/sma_feature.h"

#include <iostream>
#include <math.h>


namespace TradingBot {

    SMAFeature::SMAFeature(int period) : period(period) {}

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
        if (!incremental || !lastValue) {
            assert(period <= candles.size());
            lastValue = sum(candles, period);
            return lastValue / period;
        }
        assert(period + 1 <= candles.size());
        double diff = (candles.back().close - lastValue);
        lastValue = lastValue + diff;
        return lastValue / period;
    }

    int SMAFeature::getPeriod() const {
        return period;
    }

} // namespace TradingBot
