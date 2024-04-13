#include "strategies/strategy.h"


namespace TradingBot {

    std::ostream& operator<<(std::ostream& os, const ParamSet& paramSet) {
        for (const auto& param : paramSet) {
            if (std::holds_alternative<int>(param)) {
                os << std::get<int>(param) << " ";
            } else {
                os << std::get<double>(param) << " ";
            }
        }
        return os;
    }

    const ParamSet& Strategy::getParamSet() const {
        return paramSet;
    }

    ParamSet Strategy::getDefaultParamSet() const {
        return {};
    }

    ParamSet Strategy::getMinParamSet() const {
        return {};
    }

    ParamSet Strategy::getMaxParamSet() const {
        return {};
    }

    bool Strategy::checkParamSet(const ParamSet& paramSet) const {
        return true;
    }

    void Strategy::run() {
        do {
            step();
        } while (market->update());
    }

} // namespace TradingBot
