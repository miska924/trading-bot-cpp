#include "features/atr_feature.h"

#include <math.h>


namespace TradingBot {

    double atr(const Helpers::VectorView<Candle>& candles, int period) {
        int size = candles.size();
        int begin = size - period;
        double sum = 0;
        for (int i = size - 1; i >= begin; --i) {
            sum += pow(candles[i].high - candles[i].low, 2);
        }
        return sqrt(sum / period);
    }

    ATRFeature::ATRFeature(int period) : period(period) {}

    double ATRFeature::operator()(const Helpers::VectorView<Candle>& candles, bool incremental) {
        if (!incremental || !lastValue) {
            assert(period <= candles.size());
            return lastValue = atr(candles, period);
        }
        assert(false);
        return 0;
        // assert(period + 1 <= candles.size());
        // double diff = candles.back().close - lastValue;
        // double correction = pow(1.0 - smooth, period) * candles[candles.size() - period - 1].close;
        // return lastValue = lastValue + smooth * (diff - correction);
    }

    int ATRFeature::getPeriod() const {
        return period;
    }

} // namespace TradingBot
