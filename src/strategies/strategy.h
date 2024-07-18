#pragma once

#include <iostream>
#include <variant>
#include <vector>

#include "markets/market.h"


namespace TradingBot {

    using ParamSet = std::vector<std::variant<int, double>>;

    class Strategy {
    public:
        virtual ~Strategy() = default;
        void run();
        virtual void step() = 0;
        virtual const ParamSet& getParamSet() const;
        virtual bool checkParamSet(const ParamSet& paramSet) const;
        virtual std::vector<std::vector<std::pair<time_t, double> > > getPlots();
    protected:
        Market* market;
        ParamSet paramSet;
    };

} // namespace TradingBot


std::ostream& operator<<(std::ostream& os, const TradingBot::ParamSet& paramSet);
