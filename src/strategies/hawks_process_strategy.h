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
    const int DEFAULT_HAWKS_PROCESS_RISK = 1;
    const bool DEFAULT_HAWKS_PROCESS_PREVENT_DRAWDOWN = false;
    const double DEFAULT_HAWKS_PROCESS_PREVENT_DRAWDOWN_COEFF = 0.01;

    class HawksProcessStrategy : public Strategy {
    public:
        HawksProcessStrategy(
            Market* market,
            const ParamSet& paramSet
        );

        HawksProcessStrategy(
            Market* market = nullptr,
            int atrPeriod = DEFAULT_HAWKS_PROCESS_ATR_PERIOD,
            int normRangePeriod = DEFAULT_HAWKS_PROCESS_NORM_RANGE_PERIOD,
            int normalRangeSmoothPeriod = DEFAULT_HAWKS_PROCESS_NORM_RANGE_SMOOTH_PERIOD,
            double risk = DEFAULT_HAWKS_PROCESS_RISK,
            bool preventDrawdown = DEFAULT_HAWKS_PROCESS_PREVENT_DRAWDOWN,
            double preventDrawdownCoeff = DEFAULT_HAWKS_PROCESS_PREVENT_DRAWDOWN_COEFF
        );

        bool checkParamSet(const ParamSet& paramSet) const override;

        void step() override;

    private:
        int atrPeriod = 0;
        int normRangePeriod = 0;
        bool preventDrawdown = false;
        double risk = 1;
        int normalRangeSmoothPeriod = 0;
        double preventDrawdownCoeff = 0;

        int lastDownCrossIndex = -1;
        int lastUpCrossIndex = -1;
        double checkPointBalance;

        ATRFeature atr;

        std::set<std::pair<double, int>> normRangeSorted; // value, index
        std::deque<std::pair<double, int>> normRange; // value, index

        void updateNormRange(double value, int index);
    };

} // namespace TradingBot
