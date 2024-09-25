#include "strategies/ema_crossover_strategy.h"

#include <assert.h>

#include "helpers/vector_view.h"
#include "markets/backtest_market.h"
#include "helpers/vector_view.h"


namespace TradingBot {

    EMACrossoverStrategy::EMACrossoverStrategy(int fastPeriod, int slowPeriod) : EMACrossoverStrategy(ParamSet{fastPeriod, slowPeriod}) {}

    EMACrossoverStrategy::EMACrossoverStrategy(const ParamSet& paramSet) {
        assert(checkParamSet(paramSet));
        this->paramSet = paramSet;
        int fast = std::get<int>(paramSet[0]);
        int slow = std::get<int>(paramSet[1]);
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
    }

    bool EMACrossoverStrategy::checkParamSet(const ParamSet& paramSet) const {
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

    std::vector<std::vector<std::pair<time_t, double> > > EMACrossoverStrategy::getPlots() {
        return {fastPlot, slowPlot};
    }

    Signal EMACrossoverStrategy::step(bool newCandle) {
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

        if (savePlots) {
            fastPlot.push_back({candles.back().time, fast});
            slowPlot.push_back({candles.back().time, slow});
        }

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

    EMACrossoverAdvancedStrategy::EMACrossoverAdvancedStrategy(int fastPeriod, int slowPeriod) : EMACrossoverAdvancedStrategy(ParamSet{fastPeriod, slowPeriod}) {}

    EMACrossoverAdvancedStrategy::EMACrossoverAdvancedStrategy(const ParamSet& paramSet) {
        assert(checkParamSet(paramSet));
        this->paramSet = paramSet;
        int fast = std::get<int>(paramSet[0]);
        int slow = std::get<int>(paramSet[1]);
        fastPeriod = fast;
        slowPeriod = slow;
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
    }

    Signal EMACrossoverAdvancedStrategy::step(bool newCandle) {
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

        if (candles.back().low > fast) {
            aboveCombo++;
        } else {
            aboveCombo = 0;
        }
        if (candles.back().low < fast) {
            belowCombo++;
        } else {
            belowCombo = 0;
        }

        if (savePlots) {
            fastPlot.push_back({candles.back().time, fast});
            slowPlot.push_back({candles.back().time, slow});
        }

        if (fast > slow && previousFast < previousSlow) {
            if (aboveCombo > fastPeriod / 3) {
                return {
                    .reset = market->getBalance().assetB != 0,
                    .order = 1
                };
            } else {
                return {.reset = market->getBalance().assetB != 0};
            }
        } else if (fast < slow && previousFast > previousSlow) {
            if (belowCombo > fastPeriod / 3) {
                return {
                    .reset = market->getBalance().assetB != 0,
                    .order = -1
                };
            } else {
                return {.reset = market->getBalance().assetB != 0};
            }
        }
        return {};
    }

    EMACrossoverHoldSlowStrategy::EMACrossoverHoldSlowStrategy(int fastPeriod, int slowPeriod) {
        paramSet = {
            fastPeriod,
            slowPeriod,
        };
        hold = slowPeriod;
        assert(checkParamSet(paramSet));
        slowEMA = EMAFeature(slowPeriod);
        fastEMA = EMAFeature(fastPeriod);
    }

    EMACrossoverHoldSlowStrategy::EMACrossoverHoldSlowStrategy(const ParamSet& paramSet) {
        assert(checkParamSet(paramSet));
        this->paramSet = paramSet;
        int fast = std::get<int>(paramSet[0]);
        int slow = std::get<int>(paramSet[1]);
        hold = slow;
        fastEMA = EMAFeature(fast);
        slowEMA = EMAFeature(slow);
    }

    EMACrossoverHoldFixedStrategy::EMACrossoverHoldFixedStrategy(
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

    EMACrossoverHoldFixedStrategy::EMACrossoverHoldFixedStrategy(
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

    Signal EMACrossoverHoldFixedStrategy::step(bool newCandle) {
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

        if (savePlots) {
            fastPlot.push_back({candles.back().time, fast});
            slowPlot.push_back({candles.back().time, slow});
        }

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

    bool EMACrossoverHoldFixedStrategy::checkParamSet(const ParamSet& paramSet) const {
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

    bool EMACrossoverHoldSlowStrategy::checkParamSet(const ParamSet& paramSet) const {
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
