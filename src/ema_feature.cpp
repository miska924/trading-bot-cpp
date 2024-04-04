#include "features/ema_feature.h"


namespace TradingBot {

    EMAFeature::EMAFeature(int period, int lag) : period(period), lag(lag) {}

    std::optional<double> ema(const std::vector<Candle>& candles, int period, int lag) {
        if (period + lag > candles.size()) {
            return {};
        }

        double smooth = 2.0 / (period + 1.0);
        size_t begin = candles.size() - period - lag;
        double ema = candles[begin].close;
        for (size_t i = begin + 1; i + lag < candles.size(); i++) {
            ema = (candles[i].close - ema) * smooth + ema;
        }
        return ema;
    }

    std::optional<double> EMAFeature::operator()(const std::vector<Candle>& candles) const {
        return ema(candles, period, lag);
    }

} // namespace TradingBot