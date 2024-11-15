#include "features/bollinger_bands_feature.h"

#include <cmath>
#include <iostream>
#include <math.h>

namespace TradingBot {

    BollingerBandsFeature::BollingerBandsFeature(int period, double k) :
        period(period),
        k(k),
        smaFeature(period),
        queue([](double a, double b) -> double {
            return a + b;
        }) {}

    double BollingerBandsFeature::calculateSTD(const Helpers::VectorView<Candle>& candles, double sma) {
        double sum = 0;
        for (int i = candles.size() - period; i < candles.size(); ++i) {
            sum += std::pow(candles[i].close - sma, 2);
        }
        return std::sqrt(sum / period);
    }

    double BollingerBandsFeature::operator()(const Helpers::VectorView<Candle>& candles, bool incremental) {
        int size = candles.size();
        assert(period <= size);

        double sma = smaFeature(candles, incremental);

        double stddev;
        if (incremental) {
            if (!queue.size()) {
                for (int i = period; i > 0; --i) {
                    double close = candles[size - i].close;
                    queue.push(close * close);
                }
            } else {
                double close = candles.back().close;
                queue.push(close * close);
                if (queue.size() > period) {
                    queue.pop();
                }
            }

            double sqsm = queue.getValue();
            stddev = sqrt(std::max(0.0, sqsm / period - sma * sma));
        } else {
            stddev = calculateSTD(candles, sma);
            queue.clear();
        }
        
        return k * stddev;
    }

    int BollingerBandsFeature::getPeriod() const {
        return period;
    }

}
