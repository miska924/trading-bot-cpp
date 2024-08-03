#pragma once

#include "strategies/strategy.h"

#include "helpers/function_queue.h"


namespace TradingBot {

    class DonchainStrategy : public Strategy {
    public:
        DonchainStrategy(Market* _market, const ParamSet& paramSet);
        DonchainStrategy(Market* _market = nullptr, int period = 10);
        virtual void step() override;
        virtual bool checkParamSet(const ParamSet& paramSet) const override;
        virtual std::vector<std::vector<std::pair<time_t, double> > > getPlots() override;

    protected:
        int period;
        
        bool waitMin = true;
        bool waitMax = true;

        Helpers::FunctionQueue<double> minQueue;
        Helpers::FunctionQueue<double> maxQueue;

        //plots:
        std::vector<std::pair<time_t, double> > minPlot;
        std::vector<std::pair<time_t, double> > maxPlot;
    };

    class DonchainLastLoserStrategy : public DonchainStrategy {
    public:
        DonchainLastLoserStrategy(Market* _market, const ParamSet& paramSet);
        DonchainLastLoserStrategy(Market* _market = nullptr, int period = 10);
        virtual void step() override;

    protected:
        double lastPrice = 0;
    };

} // namespace TradingBot
