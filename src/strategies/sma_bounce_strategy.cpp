#include "strategies/sma_bounce_strategy.cpp"


namespace TradingBot {

    SMABounceStrategy::SMABounceStrategy(
        Market* market, int period, double gap, int holdCandles
    ) : period(period), gap(gap), holdCandles(holdCandles) {
        this->market = market;
    }

    SMABounceStrategy::SMABounceStrategy(
        Market* market,
        const ParamSet& paramSet
    ) : SMABounceStrategy(
        market,
        std::get<int>(paramSet[0]),
        std::get<double>(paramSet[1]),
        std::get<int>(paramSet[2])
    ) {}

    bool SMABounceStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 3) {
            return false;
        }

        const int* period = std::get_if<int>(&paramSet[0]);
        const double* gap = std::get_if<double>(&paramSet[1]);
        const int* holdCandles = std::get_if<int>(&paramSet[2]);
        if (period == nullptr || gap == nullptr || holdCandles == nullptr) {
            return false;
        }
        if (*period < 1 || *gap < 0 || *holdCandles < 1) {
            return false;
        }
        return true;
    }

    void SMABounceStrategy::step() {
        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() <= period) {
            return;
        }
        sma = SMAFeature(candles, period);

        if ()
    }

} // namespace TradingBot
