#include "strategies/strategy.h"


namespace TradingBot {

    const ParamSet& Strategy::getParamSet() const {
        return paramSet;
    }

    bool Strategy::checkParamSet(const ParamSet& paramSet) const {
        return true;
    }

    void Strategy::run() {
        do {
            // if (market->getCandles().size() % (24 * 30) == 0) {
            //     double got = market->getBalance().asAssetA() - 100.0;
            //     std::cerr << "got: " << std::max(0.0, got) << std::endl;
            //     if (market->getBalance().asAssetA() > 100) {
            //         market->payOut(got);
            //     }
            // }
            step();
        } while (market->update());
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
