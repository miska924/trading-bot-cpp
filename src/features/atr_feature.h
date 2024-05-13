#pragma once

#include "features/feature.h"


namespace TradingBot {

    const int DEFAULT_ATR_PERIOD = 100;

    class ATRFeature : public Feature {
    public:
        ATRFeature(int period = DEFAULT_ATR_PERIOD);
        int getPeriod() const;
        double operator()(
            const Helpers::VectorView<Candle>& candles,
            bool incremental = false
        ) override;
    private:
        int period;
        double lastValue = 0;
    };

} // namespace TradingBot
