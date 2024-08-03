#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <optional>

#include "markets/market.h"


namespace TradingBot {

    struct Signal {
        bool reset = false;
        double order = 0.0;
    };

    using ParamSet = std::vector<std::variant<int, double>>;

    class Strategy {
    public:
        virtual ~Strategy() = default;
        void attachMarketInfo(MarketInfo* marketInfo);
        virtual void onMarketInfoAttach();
        virtual Signal step(bool newCandle) = 0;
        virtual const ParamSet& getParamSet() const;
        virtual bool checkParamSet(const ParamSet& paramSet) const;
        virtual std::vector<std::vector<std::pair<time_t, double> > > getPlots();
    protected:
        MarketInfo* market;
        ParamSet paramSet;
    };

} // namespace TradingBot


std::ostream& operator<<(std::ostream& os, const TradingBot::ParamSet& paramSet);
