#include "features/ema_feature.h"

#include <iostream>
#include <math.h>


namespace TradingBot {

    EMAFeature::EMAFeature(int period) :
        period(period),
        smooth(2.0 / (period + 1.0)),
        queue(1.0 - smooth) {}

    double ema(const Helpers::VectorView<Candle>& candles, int period, double smooth) {
        int size = candles.size();
        int begin = size - period;
        double ema = 0;
        double alphaPow = 1.0;
        for (int i = size - 1; i >= begin; --i) {
            ema += candles[i].close * alphaPow;
            alphaPow *= (1.0 - smooth);
        }

        // incorrect: return ema * smooth;
        return ema * smooth * (1.0 + alphaPow);
    }

    double EMAFeature::operator()(const Helpers::VectorView<Candle>& candles, bool incremental) {
        int size = candles.size();
        assert(period <= size);

        if (!incremental) {
            return ema(candles, period, smooth);
        }

        if (!queue.size()) {
            for (int i = period; i > 0; --i) {
                queue.push(candles[size - i].close);
            }
        } else {
            queue.push(candles.back().close);
            queue.pop();
        }

        return queue.getValue() * smooth * (1.0 + queue.powerK(period));

        assert(period + 1 <= candles.size());
        double diff = candles.back().close - lastValue;
        double correction = candles[candles.size() - period - 1].close * pow(smooth, period);

        return lastValue = lastValue + smooth * (diff - correction);
    }

    int EMAFeature::getPeriod() const {
        return period;
    }

} // namespace TradingBot
