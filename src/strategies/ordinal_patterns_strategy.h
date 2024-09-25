#pragma once

#include "strategies/strategy.h"


namespace TradingBot {

    const int DEFAULT_ORDINAL_PATTERNS_FIT_PERIOD = 1000;
    const int DEFAULT_ORDINAL_PATTERNS_ORDER_PERIOD = 5;

    class OrdinalPatternsStrategy : public Strategy {
    public:
        OrdinalPatternsStrategy(const ParamSet& paramSet);
        OrdinalPatternsStrategy(
            int fitPeriod = DEFAULT_ORDINAL_PATTERNS_FIT_PERIOD,
            int orderPeriod = DEFAULT_ORDINAL_PATTERNS_ORDER_PERIOD
        );

        virtual Signal step(bool newCandle) override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;
    protected:
        int fitPeriod;
        int orderPeriod;
        int wait = 0;
    };

} // namespace TradingBot
