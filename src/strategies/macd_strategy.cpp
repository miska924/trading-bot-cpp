#include "strategies/macd_strategy.h"

#include <assert.h>

#include "helpers/vector_view.h"
#include "markets/backtest_market.h"
#include "helpers/vector_view.h"


namespace TradingBot {

    MACDStrategy::MACDStrategy(int fastPeriod, int slowPeriod) {
        paramSet = {
            fastPeriod,
            slowPeriod,
        };
        assert(checkParamSet(paramSet));
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
    }

    MACDStrategy::MACDStrategy(const ParamSet& paramSet) {
        assert(checkParamSet(paramSet));
        this->paramSet = paramSet;
        int fast = std::get<int>(paramSet[0]);
        int slow = std::get<int>(paramSet[1]);
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
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

    Signal MACDStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        Helpers::VectorView<Candle> candles = market->getCandles();

        if (candles.size() <= slowEMA.getPeriod()) {
            fast = slow = 0;
            return {};
        }

        double previousFast;
        double previousSlow;
        if (fast && slow) {
            previousFast = fast;
            previousSlow = slow;
        } else {
            previousFast = fastEMA(candles.subView(0, candles.size() - 1));
            previousSlow = slowEMA(candles.subView(0, candles.size() - 1));
        }
        fast = fastEMA(candles, true);
        slow = slowEMA(candles, true);

        if (fast > slow && previousFast < previousSlow) {
            return {
                .reset = true,
                .order = 1
            };
        } else if (fast < slow && previousFast > previousSlow) {
            return {
                .reset = true,
                .order = -1
            };
        }
        return {};
    }

    MACDHoldSlowStrategy::MACDHoldSlowStrategy(int fastPeriod, int slowPeriod) {
        paramSet = {
            fastPeriod,
            slowPeriod,
        };
        hold = slowPeriod;
        assert(checkParamSet(paramSet));
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
    }

    MACDHoldSlowStrategy::MACDHoldSlowStrategy(const ParamSet& paramSet) {
        assert(checkParamSet(paramSet));
        this->paramSet = paramSet;
        int fast = std::get<int>(paramSet[0]);
        int slow = std::get<int>(paramSet[1]);
        hold = slow;
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
    }

    MACDHoldFixedStrategy::MACDHoldFixedStrategy(
        const ParamSet& paramSet
    ) {
        assert(checkParamSet(paramSet));
        this->paramSet = paramSet;
        int fast = std::get<int>(paramSet[0]);
        int slow = std::get<int>(paramSet[1]);
        hold = std::get<int>(paramSet[2]);
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
    }

    MACDHoldFixedStrategy::MACDHoldFixedStrategy(
        int fastPeriod,
        int slowPeriod,
        int holdCandles
    ) : hold(holdCandles) {
        paramSet = {
            fastPeriod,
            slowPeriod,
            holdCandles,
        };
        assert(checkParamSet(paramSet));
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
    }

    Signal MACDHoldFixedStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }
        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() <= slowEMA.getPeriod()) {
            fast = slow = 0;
            return {};
        }

        if (market->time() < waitUntill) {
            fast = fastEMA(candles, true);
            slow = slowEMA(candles, true);
            return {};
        }
        
        bool reset = false;

        if (market->getBalance().assetB != 0) {
            reset = true;
        }

        double previousFast;
        double previousSlow;
        if (fast && slow) {
            previousFast = fast;
            previousSlow = slow;
        } else {
            previousFast = fastEMA(candles.subView(0, candles.size() - 1));
            previousSlow = slowEMA(candles.subView(0, candles.size() - 1));
        }
        fast = fastEMA(candles, true);
        slow = slowEMA(candles, true);

        time_t wait = hold * market->getCandleTimeDelta();
        if (fast > slow && previousFast < previousSlow) {
            waitUntill = market->time() + wait;
            return {
                .reset = reset,
                .order = 1
            };
        } else if (fast < slow && previousFast > previousSlow) {
            waitUntill = market->time() + wait;
            return {
                .reset = reset,
                .order = -1
            };
        }
        return {.reset = reset};
    }

    bool MACDHoldFixedStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 3) {
            return false;
        }

        const int* fast = std::get_if<int>(&paramSet[0]);
        const int* slow = std::get_if<int>(&paramSet[1]);
        const int* hold = std::get_if<int>(&paramSet[2]);
        if (fast == nullptr || slow == nullptr || hold == nullptr) {
            return false;
        }
        if (*fast >= *slow || *fast < 1 || *hold < 1) {
            return false;
        }

        return true;
    }

    bool MACDHoldSlowStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 2) {
            return false;
        }

        const int* fast = std::get_if<int>(&paramSet[0]);
        const int* slow = std::get_if<int>(&paramSet[1]);
        if (fast == nullptr || slow == nullptr) {
            return false;
        }
        if (*fast >= *slow || *fast < 1) {
            return false;
        }

        return true;
    }

} // namespace TradingBot
