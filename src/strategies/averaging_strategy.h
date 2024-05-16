#pragma once

#include "strategies/strategy.h"
#include "markets/market.h"
#include "features/atr_feature.h"


namespace TradingBot {

    const int DEFAULT_AVERAGING_ATR_PERIOD = 4 * 24;
    const int DEFAULT_AVERAGING_FORCE_CLOSE_CANDLES = 10000;
    const double DEFAULT_AVERAGING_COEFF = 30;
    const double DEFAULT_AVERAGING_RISK = 0.5;
    const double DEFAULT_AVERAGING_POSITION_SIDE_PERIOD = 10;

    class AveragingStrategy : public Strategy {
    public:
        AveragingStrategy(Market* market, const ParamSet& paramSet);
        AveragingStrategy(
            Market* market = nullptr,
            int atrPeriod = DEFAULT_AVERAGING_ATR_PERIOD,
            int positionSidePeriod = DEFAULT_AVERAGING_POSITION_SIDE_PERIOD,
            int waitCandles = DEFAULT_AVERAGING_FORCE_CLOSE_CANDLES,
            double coeff = DEFAULT_AVERAGING_COEFF,
            double risk = DEFAULT_AVERAGING_RISK
        );

        virtual void step() override;

        virtual bool checkParamSet(const ParamSet& paramSet) const override;

    private:
        double risk;
        int position = 0;
        bool inCombo = false;
        double positionsSum = 0;
        int atrPeriod = 0;
        int positionSidePeriod = 0;
        double savedAtr = 0;
        double atrValue = 0;
        int noUpdateCount = 0;

        int positionsLimit = 8;

        Order lastOrder = {.side = OrderSide::RESET};

        ATRFeature atr;

        time_t waitUntill = 0;
        int waitCandles = 0;
        double coeff = 0;

    };

} // namespace TradingBot
