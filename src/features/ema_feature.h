#pragma once

#include <vector>

#include "features/feature.h"
#include "helpers/vector_view.h"
#include "helpers/function_queue.h"


namespace TradingBot {

    const int DEFAULT_EMA_PERIOD = 15;

    class EMAFeature : public Feature {
    public:
        EMAFeature(int period = DEFAULT_EMA_PERIOD);
        int getPeriod() const;
        virtual double operator()(const Helpers::VectorView<Candle>& candles, bool incremental = false) override;
    private:
        int period;
        double smooth;
        double lastValue = 0;
        Helpers::WeightedSumQueue<double> queue;
    };

} // namespace TradingBot
