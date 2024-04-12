#include "strategies/macd_strategy.h"

#include <assert.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "helpers/vector_view.h"


namespace TradingBot {

    MACDStrategy::MACDStrategy(Market* _market, int fastPeriod, int slowPeriod) {
        assert(fastPeriod < slowPeriod);
        market = _market;
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
        previousSlowEMA = EMAFeature(slowPeriod, 1);
        previousFastEMA = EMAFeature(fastPeriod, 1);
        paramSet = {
            fastPeriod,
            slowPeriod
        };
    }

    MACDStrategy::MACDStrategy(Market* _market, const ParamSet& _paramSet) {
        market = _market;
        paramSet = _paramSet;
        int fast = std::get<int>(_paramSet[0]);
        int slow = std::get<int>(_paramSet[1]);
        assert(fast < slow);
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
        previousFastEMA = EMAFeature(fast, 1);
        previousSlowEMA = EMAFeature(slow, 1);
    }

    ParamSet MACDStrategy::getDefaultParamSet() const {
        return {
            DEFAULT_MACD_FAST_PERIOD,
            DEFAULT_MACD_SLOW_PERIOD,
        };
    }
    ParamSet MACDStrategy::getMinParamSet() const {
        return {
            MIN_MACD_FAST_PERIOD,
            MIN_MACD_SLOW_PERIOD,
        };
    }
    ParamSet MACDStrategy::getMaxParamSet() const {
        return {
            MAX_MACD_FAST_PERIOD,
            MAX_MACD_SLOW_PERIOD,
        };
    }

    bool MACDStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 2) {
            return false;
        }

        const int* fast = std::get_if<int>(&paramSet[0]);
        const int* slow = std::get_if<int>(&paramSet[1]);
        if (fast == nullptr || slow == nullptr) {
            return false;
        }
        if (*fast >= *slow) {
            return false;
        }

        return true;
    }

    void MACDStrategy::step() {
        Helpers::VectorView<Candle> candles = market->getCandles();
        std::optional<double> fastOptional = fastEMA(candles);
        std::optional<double> slowOptional = slowEMA(candles);
        std::optional<double> previousFastOptional = previousFastEMA(candles);
        std::optional<double> previousSlowOptional = previousSlowEMA(candles);
        if (!fastOptional || !slowOptional || !previousFastOptional || !previousSlowOptional) {
            return;
        }

        double fast = *fastOptional;
        double slow = *slowOptional;
        double previousFast = *previousFastOptional;
        double previousSlow = *previousSlowOptional;

        if (fast > slow && previousFast < previousSlow) {
            market->order({.side = OrderSide::RESET});
            market->order({.side = OrderSide::BUY, .amount = 1});
        } else if (fast < slow && previousFast > previousSlow) {
            market->order({.side = OrderSide::RESET});
            market->order({.side = OrderSide::SELL, .amount = 1});
        }
    }

    MACDHoldSlowStrategy::MACDHoldSlowStrategy(Market* _market, int fastPeriod, int slowPeriod) {
        assert(fastPeriod < slowPeriod);
        market = _market;
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
        previousSlowEMA = EMAFeature(slowPeriod, 1);
        previousFastEMA = EMAFeature(fastPeriod, 1);
        paramSet = {
            fastPeriod,
            slowPeriod
        };
    }

    MACDHoldSlowStrategy::MACDHoldSlowStrategy(Market* _market, const ParamSet& _paramSet) {
        market = _market;
        paramSet = _paramSet;
        int fast = std::get<int>(_paramSet[0]);
        int slow = std::get<int>(_paramSet[1]);
        assert(fast < slow);
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
        previousFastEMA = EMAFeature(fast, 1);
        previousSlowEMA = EMAFeature(slow, 1);
    }

    void MACDHoldSlowStrategy::step() {
        if (market->time() < waitUntill) {
            return;
        }
        if (market->getBalance().assetB != 0) {
            market->order({.side = OrderSide::RESET});
        }

        Helpers::VectorView<Candle> candles = market->getCandles();

        std::optional<double> fastOptional = fastEMA(candles);
        std::optional<double> slowOptional = slowEMA(candles);
        std::optional<double> previousFastOptional = previousFastEMA(candles);
        std::optional<double> previousSlowOptional = previousSlowEMA(candles);
        
        if (
            !fastOptional ||
            !slowOptional ||
            !previousFastOptional ||
            !previousSlowOptional
        ) {
            return;
        }

        double fast = *fastOptional;
        double slow = *slowOptional;
        double previousFast = *previousFastOptional;
        double previousSlow = *previousSlowOptional;

        time_t wait = slowEMA.getPeriod() * market->getCandleTimeDelta();

        if (fast > slow && previousFast < previousSlow) {
            market->order({.side = OrderSide::BUY, .amount = 1});
            waitUntill = market->time() + wait;
        } else if (fast < slow && previousFast > previousSlow) {
            market->order({.side = OrderSide::SELL, .amount = 1});
            waitUntill = market->time() + wait;
        }
    }

    MACDHoldFixedCandlesStrategy::MACDHoldFixedCandlesStrategy(
        Market* _market,
        const ParamSet& _paramSet
    ) {
        market = _market;
        paramSet = _paramSet;
        int fast = std::get<int>(_paramSet[0]);
        int slow = std::get<int>(_paramSet[1]);
        assert(fast < slow);
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
        previousFastEMA = EMAFeature(fast, 1);
        previousSlowEMA = EMAFeature(slow, 1);
    }

    MACDHoldFixedCandlesStrategy::MACDHoldFixedCandlesStrategy(
        Market* _market,
        int fastPeriod,
        int slowPeriod,
        int holdCandles
    ) : hold(holdCandles) {
        assert(fastPeriod < slowPeriod);
        market = _market;
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
        previousSlowEMA = EMAFeature(slowPeriod, 1);
        previousFastEMA = EMAFeature(fastPeriod, 1);
    }

    void MACDHoldFixedCandlesStrategy::step() {
        if (market->time() < waitUntill) {
            return;
        }
        if (market->getBalance().assetB != 0) {
            market->order({.side = OrderSide::RESET});
        }

        Helpers::VectorView<Candle> candles = market->getCandles();

        std::optional<double> fastOptional = fastEMA(candles);
        std::optional<double> slowOptional = slowEMA(candles);
        std::optional<double> previousFastOptional = previousFastEMA(candles);
        std::optional<double> previousSlowOptional = previousSlowEMA(candles);
        
        if (
            !fastOptional ||
            !slowOptional ||
            !previousFastOptional ||
            !previousSlowOptional
        ) {
            return;
        }

        double fast = *fastOptional;
        double slow = *slowOptional;
        double previousFast = *previousFastOptional;
        double previousSlow = *previousSlowOptional;

        time_t wait = hold * market->getCandleTimeDelta();
        if (fast > slow && previousFast < previousSlow) {
            market->order({.side = OrderSide::BUY, .amount = 1});
            waitUntill = market->time() + wait;
        } else if (fast < slow && previousFast > previousSlow) {
            market->order({.side = OrderSide::SELL, .amount = 1});
            waitUntill = market->time() + wait;
        }
    }

    ParamSet MACDHoldFixedCandlesStrategy::getDefaultParamSet() const {
        return {
            DEFAULT_MACD_FAST_PERIOD,
            DEFAULT_MACD_SLOW_PERIOD,
            DEFAULT_MACD_HOLD_CANDLES,
        };
    }

    ParamSet MACDHoldFixedCandlesStrategy::getMinParamSet() const {
        return {
            MIN_MACD_FAST_PERIOD,
            MIN_MACD_SLOW_PERIOD,
            MIN_MACD_HOLD_CANDLES,
        };
    }

    ParamSet MACDHoldFixedCandlesStrategy::getMaxParamSet() const {
        return {
            MAX_MACD_FAST_PERIOD,
            MAX_MACD_SLOW_PERIOD,
            MAX_MACD_HOLD_CANDLES,
        };
    }

    bool MACDHoldFixedCandlesStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 3) {
            return false;
        }

        const int* fast = std::get_if<int>(&paramSet[0]);
        const int* slow = std::get_if<int>(&paramSet[1]);
        if (fast == nullptr || slow == nullptr) {
            return false;
        }
        if (*fast >= *slow) {
            return false;
        }

        return true;
    }

} // namespace TradingBot
