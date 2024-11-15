#pragma once

#include "features/feature.h"
#include "helpers/function_queue.h"


namespace TradingBot {

    const int DEFAULT_ATR_PERIOD = 100;

    class ATRFeature : public Feature {
    public:
        ATRFeature(int period = DEFAULT_ATR_PERIOD, bool logPrice = false);
        int getPeriod() const;
        double operator()(
            const Helpers::VectorView<Candle>& candles,
            bool incremental = false
        ) override;
    private:
        bool logPrice;
        int period;
        Helpers::FunctionQueue<double> queue;
    };

} // namespace TradingBot
