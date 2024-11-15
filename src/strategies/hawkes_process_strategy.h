#pragma once

#include <set>
#include <deque>
#include <math.h>

#include "markets/market.h"
#include "strategies/strategy.h"
#include "features/atr_feature.h"
#include "helpers/vector_view.h"


namespace TradingBot {

    const int DEFAULT_HAWKS_PROCESS_ATR_PERIOD = 10000;
    const int DEFAULT_HAWKS_PROCESS_NORM_RANGE_PERIOD = 10000;
    const int DEFAULT_HAWKS_PROCESS_NORM_RANGE_SMOOTH_PERIOD = 50;

    class HawkesProcessStrategy : public Strategy {
    public:
        HawkesProcessStrategy(
            const ParamSet& paramSet
        );

        HawkesProcessStrategy(
            int atrPeriod = DEFAULT_HAWKS_PROCESS_ATR_PERIOD,
            int normRangePeriod = DEFAULT_HAWKS_PROCESS_NORM_RANGE_PERIOD,
            int normalRangeSmoothPeriod = DEFAULT_HAWKS_PROCESS_NORM_RANGE_SMOOTH_PERIOD
        );

        bool checkParamSet(const ParamSet& paramSet) const override;

        Signal step(bool newCandle) override;

    private:
        int atrPeriod = 0;
        int normRangePeriod = 0;
        int normalRangeSmoothPeriod = 0;

        int lastDownCrossIndex = -1;
        int lastUpCrossIndex = -1;

        ATRFeature atr;

        std::set<std::pair<double, int>> normRangeSorted; // value, index
        std::deque<std::pair<double, int>> normRange; // value, index

        void updateNormRange(double value, int index);
    };

} // namespace TradingBot
