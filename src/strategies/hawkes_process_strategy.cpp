#include "hawkes_process_strategy.h"


namespace TradingBot {

    HawkesProcessStrategy::HawkesProcessStrategy(
        const ParamSet& paramSet
    )
        : Strategy(paramSet)
        , atrPeriod(std::get<int>(paramSet[0]))
        , normRangePeriod(std::get<int>(paramSet[1]))
        , normalRangeSmoothPeriod(std::get<int>(paramSet[2]))
        , atr(atrPeriod)
    {}

    HawkesProcessStrategy::HawkesProcessStrategy(
        int atrPeriod,
        int normRangePeriod,
        int normalRangeSmoothPeriod
    )
        : Strategy({atrPeriod, normRangePeriod, normalRangeSmoothPeriod})
        , atrPeriod(atrPeriod)
        , normRangePeriod(normRangePeriod)
        , normalRangeSmoothPeriod(normalRangeSmoothPeriod)
        , atr(atrPeriod)
    {}

    Signal HawkesProcessStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        Helpers::VectorView<Candle> candles = market->getCandles();

        if (candles.size() < atrPeriod + normRangePeriod - 1) {
            return {};
        }

        if (normRange.size() == normRangePeriod) {
            double atrValue = atr(candles, true);
            double logDiff = (std::log1p(candles.back().high) - std::log1p(candles.back().low));
            double normRangeValue = logDiff / atrValue;
            updateNormRange(normRangeValue, candles.size() - 1);
        }

        while (normRange.size() < normRangePeriod) {
            int rightBound = candles.size() - normRangePeriod + normRange.size() + 1;

            double atrValue = atr(candles.subView(0, rightBound), true);
            double logDiff = (std::log1p(candles[rightBound - 1].high) - std::log1p(candles[rightBound - 1].low));
            double normRangeValue = logDiff / atrValue;
            updateNormRange(normRangeValue, rightBound - 1);
        }

        auto percentile05 = normRangeSorted.begin();
        auto percentile95 = normRangeSorted.rend();

        for (int i = 0; i < normRangePeriod / 20; ++i) {
            percentile05++;
            percentile95++;
        }

        bool cross = false;

        double normalRangeAverage = 0;
        for (int i = 0; i < normalRangeSmoothPeriod; ++i) {
            normalRangeAverage += normRange[normRange.size() - 1 - i].first;
        }
        normalRangeAverage /= normalRangeSmoothPeriod;


        if (normalRangeAverage > percentile95->first) {
            lastUpCrossIndex = candles.size() - 1;
            cross = true;
        }
        if (normalRangeAverage < percentile05->first) {
            lastDownCrossIndex = candles.size() - 1;
            cross = true;
        }

        if (!cross || lastUpCrossIndex == -1 || lastDownCrossIndex == -1) {
            return {};
        }

        if (lastDownCrossIndex > lastUpCrossIndex) {
            if (market->getBalance().assetB != 0) {
                return {
                    .reset = true
                };
            }
            return {};
        }

        if (market->getBalance().assetB != 0) {
            return {};
        }

        if (candles[lastDownCrossIndex].close < candles[lastUpCrossIndex].close) {
            return {
                .order = 1
            };
        } else {
            return {
                .order = -1
            };
        }
        return {};
    }

    bool HawkesProcessStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 3) {
            return false;
        }

        const int* atrPeriod = std::get_if<int>(&paramSet[0]);
        const int* normRangePeriod = std::get_if<int>(&paramSet[1]);
        const int* normalRangeSmoothPeriod = std::get_if<int>(&paramSet[2]);
        if (atrPeriod == nullptr ||
            normRangePeriod == nullptr ||
            normalRangeSmoothPeriod == nullptr
        ) {
            return false;
        }
        if (*atrPeriod < 1 || *normRangePeriod < 1 || *normalRangeSmoothPeriod < 1) {
            return false;
        }
        if (*normRangePeriod < *normalRangeSmoothPeriod) {
            return false;
        }
        return true;
    }

    void HawkesProcessStrategy::updateNormRange(double value, int index) {
        normRangeSorted.insert({value, index});
        normRangeSorted.erase(normRange.front());
        normRange.push_back({value, index});
        if (normRange.size() > normRangePeriod) {
            normRange.pop_front();
        }
    }

} // namespace TradingBot
