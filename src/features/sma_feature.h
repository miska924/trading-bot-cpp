#pragma once

#include <vector>

#include "features/feature.h"
#include "helpers/vector_view.h"


namespace TradingBot {

    const int DEFAULT_SMA_PERIOD = 15;

    class SMAFeature : public Feature {
    public:
        SMAFeature(int period = DEFAULT_SMA_PERIOD);
        int getPeriod() const;
        virtual double operator()(const Helpers::VectorView<Candle>& candles, bool incremental = false) override;
    private:
        int period;
        double lastValue = 0;
    };

} // namespace TradingBot
