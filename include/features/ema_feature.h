#pragma once

#include <vector>

#include "features/feature.h"


namespace TradingBot {

    const int DEFAULT_EMA_PERIOD = 15;

    class EMAFeature : public Feature {
    public:
        EMAFeature(int period = DEFAULT_EMA_PERIOD, int lag = 0);
        virtual std::optional<double> operator()(const std::vector<Candle>& candles) const override;
    private:
        int period;
        int lag;
    };

} // namespace TradingBot
