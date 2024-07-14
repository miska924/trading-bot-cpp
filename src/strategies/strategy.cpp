#include "strategies/strategy.h"


namespace TradingBot {

    std::vector<std::vector<std::pair<time_t, double> > > Strategy::getPlots() {
        return {};
    }

    void Strategy::attachMarketInfo(MarketInfo* marketInfo) {
        market = marketInfo;
        onMarketInfoAttach();
    }

    void Strategy::onMarketInfoAttach() {}

    const ParamSet& Strategy::getParamSet() const {
        return paramSet;
    }

    bool Strategy::checkParamSet(const ParamSet& paramSet) const {
        return true;
    }

} // namespace TradingBot


std::ostream& operator<<(std::ostream& os, const TradingBot::ParamSet& paramSet) {
    for (const auto& param : paramSet) {
        if (std::holds_alternative<int>(param)) {
            os << std::get<int>(param) << " ";
        } else {
            os << std::get<double>(param) << " ";
        }
    }
    return os;
}
