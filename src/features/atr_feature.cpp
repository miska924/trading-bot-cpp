#include "features/atr_feature.h"

#include <math.h>


namespace TradingBot {

    double ATRFeature::atr(const Helpers::VectorView<Candle>& candles) {
        int size = candles.size();
        int begin = size - period;
        double sum = 0;
        for (int i = size - 1; i >= begin; --i) {
            double diff = candles[i].high - candles[i].low;
            sum += diff * diff;
        }
        savedSum = sum;
        return sqrt(sum / period);
    }

    ATRFeature::ATRFeature(int period, bool log) : period(period), log(log) {}

    double ATRFeature::operator()(const Helpers::VectorView<Candle>& candles, bool incremental) {
        if (!incremental || !lastValue) {
            assert(period <= candles.size());
            return lastValue = atr(candles);
        }
        assert(period < candles.size());
        int removeId = candles.size() - period - 1;
        double add;
        double remove;
        if (log) {
            add = std::log1p(candles.back().high) - std::log1p(candles.back().low);
            remove = std::log1p(candles[removeId].high) - std::log1p(candles[removeId].low);
        } else {
            add = candles.back().high - candles.back().low;
            remove = candles[removeId].high - candles[removeId].low;
        }
        savedSum = (savedSum + add * add - remove * remove);
        return lastValue = sqrt(savedSum / period);
    }

    int ATRFeature::getPeriod() const {
        return period;
    }

} // namespace TradingBot
