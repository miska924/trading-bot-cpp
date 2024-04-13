#pragma once

#include <vector>

#include "features/feature.h"
#include "helpers/vector_view.h"


namespace TradingBot {

    const int DEFAULT_EMA_PERIOD = 15;

    class EMAFeature : public Feature {
    public:
        EMAFeature(int period = DEFAULT_EMA_PERIOD, int lag = 0);
        int getPeriod() const;
        int getLag() const;
        virtual std::optional<double> operator()(const Helpers::VectorView<Candle>& candles) const override;
    private:
        int period;
        int lag;
    };

} // namespace TradingBot
