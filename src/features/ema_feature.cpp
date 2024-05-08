#include "features/ema_feature.h"

#include <iostream>
#include <math.h>


namespace TradingBot {

    EMAFeature::EMAFeature(int period) : period(period), smooth(2.0 / (period + 1.0)) {}

    double ema(const Helpers::VectorView<Candle>& candles, int period, double smooth) {
        int size = candles.size();
        int begin = size - period;
        double ema = 0;
        double alphaPow = 1.0;
        for (int i = size - 1; i >= begin; --i) {
            ema += candles[i].close * alphaPow;
            alphaPow *= (1.0 - smooth);
        }
        return ema * smooth;
    }

    double EMAFeature::operator()(const Helpers::VectorView<Candle>& candles, bool incremental) {
        if (!incremental || !lastValue) {
            assert(period <= candles.size());
            return lastValue = ema(candles, period, smooth);     
        }
        assert(period + 1 <= candles.size());
        double diff = candles.back().close - lastValue;
        double correction = pow(1.0 - smooth, period) * candles[candles.size() - period - 1].close;
        return lastValue = lastValue + smooth * (diff - correction);
        
    }

    int EMAFeature::getPeriod() const {
        return period;
    }

} // namespace TradingBot
