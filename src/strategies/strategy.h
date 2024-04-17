#pragma once

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
        virtual ParamSet getDefaultParamSet() const;
        virtual ParamSet getMinParamSet() const;
        virtual ParamSet getMaxParamSet() const;
        virtual bool checkParamSet(const ParamSet& paramSet) const;
    protected:
        Market* market;
        ParamSet paramSet;
    };

} // namespace TradingBot