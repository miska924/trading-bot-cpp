#pragma once

#include "strategies/strategy.h"
#include "markets/market.h"
#include "features/atr_feature.h"


namespace TradingBot {

    const int DEFAULT_AVERAGING_PERIOD = 4 * 24;
    const int DEFAULT_AVERAGING_WAIT_CANDLES = 1000;
    const double DEFAULT_AVERAGING_COEFF = 20;

    class AveragingStrategy : public Strategy {
    public:
        AveragingStrategy(Market* market, const ParamSet& paramSet);
        AveragingStrategy(
            Market* market,
            int period = DEFAULT_AVERAGING_PERIOD,
            int waitCandles = DEFAULT_AVERAGING_WAIT_CANDLES,
            double coeff = DEFAULT_AVERAGING_COEFF
        );

        virtual void step() override;

        virtual bool checkParamSet(const ParamSet& paramSet) const override;

    private:
        double risk = 0.5;
        int position = 0;
        bool inCombo = false;
        double noLoss = 0;
        int period = 0;
        double savedAtr = 0;

        int positionsLimit = 15;

        Order lastOrder = {.side = OrderSide::RESET};

        ATRFeature atr;

        time_t waitUntill = 0;
        int waitCandles = 0;
        double coeff = 0;

    };

} // namespace TradingBot
