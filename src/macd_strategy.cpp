#include "strategies/macd_strategy.h"


namespace TradingBot {

    MACDStrategy::MACDStrategy(Market* market, int fastPeriod, int slowPeriod) : market(market) {
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
        previousSlowEMA = EMAFeature(slowPeriod, 1);
        previousFastEMA = EMAFeature(fastPeriod, 1);
    }

    void MACDStrategy::run() {
        do {
            const std::vector<Candle>& candles = market->getCandles();
            std::optional<double> fastOptional = fastEMA(candles);
            std::optional<double> slowOptional = slowEMA(candles);
            std::optional<double> previousFastOptional = previousFastEMA(candles);
            std::optional<double> previousSlowOptional = previousSlowEMA(candles);
            if (!fastOptional || !slowOptional || !previousFastOptional || !previousSlowOptional) {
                continue;
            }

            double fast = *fastOptional;
            double slow = *slowOptional;
            double previousFast = *previousFastOptional;
            double previousSlow = *previousSlowOptional;


            if (fast > slow && previousFast < previousSlow) {
                market->order({.side = OrderSide::BUY, .amount = 1});
            } else if (fast < slow && previousFast > previousSlow) {
                market->order({.side = OrderSide::SELL, .amount = 1});
            }
        } while (market->update());
    }

} // namespace TradingBot
