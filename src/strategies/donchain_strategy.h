#pragma once

#include "strategies/strategy.h"

#include "helpers/function_queue.h"


namespace TradingBot {

    class DonchainStrategy : public Strategy {
    public:
        DonchainStrategy(const ParamSet& paramSet);
        DonchainStrategy(int period = 10);
        virtual Signal step(bool newCandle) override;
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
        DonchainLastLoserStrategy(const ParamSet& paramSet);
        DonchainLastLoserStrategy(int period = 10);
        virtual Signal step(bool newCandle) override;

    protected:
        double lastPrice = 0;
    };

} // namespace TradingBot
