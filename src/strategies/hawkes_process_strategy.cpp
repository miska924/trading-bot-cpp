#include "hawkes_process_strategy.h"


namespace TradingBot {

    HawkesProcessStrategy::HawkesProcessStrategy(
        const ParamSet& paramSet
    ) {
        assert(checkParamSet(paramSet));
        atrPeriod = std::get<int>(paramSet[0]);
        normRangePeriod = std::get<int>(paramSet[1]);
        normalRangeSmoothPeriod = std::get<int>(paramSet[2]);
        risk = std::get<double>(paramSet[3]);
        preventDrawdown = std::get<int>(paramSet[4]);
        preventDrawdownCoeff = std::get<double>(paramSet[5]);
        atr = ATRFeature(atrPeriod, true);
    }

    HawkesProcessStrategy::HawkesProcessStrategy(
        int atrPeriod,
        int normRangePeriod,
        int normalRangeSmoothPeriod,
        double risk,
        bool preventDrawdown,
        double preventDrawdownCoeff
    ) :
        atrPeriod(atrPeriod),
        normRangePeriod(normRangePeriod),
        normalRangeSmoothPeriod(normalRangeSmoothPeriod),
        risk(risk),
        preventDrawdown(preventDrawdown),
        preventDrawdownCoeff(preventDrawdownCoeff)
    {
        paramSet = {atrPeriod, normRangePeriod, normalRangeSmoothPeriod, risk, preventDrawdown, preventDrawdownCoeff};
        assert(checkParamSet(paramSet));
        atr = ATRFeature(atrPeriod, true);
    }

    void HawkesProcessStrategy::onMarketInfoAttach() {
        checkPointBalance = market->getBalance().asAssetA();
    }

    Signal HawkesProcessStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        Helpers::VectorView<Candle> candles = market->getCandles();

        if (candles.size() < atrPeriod + normRangePeriod - 1) {
            return {};
        }

        // checkPointBalance = std::max(checkPointBalance, market->getBalance().asAssetA());
        if (preventDrawdown) {
            double drawdown = (checkPointBalance - market->getBalance().asAssetA()) / checkPointBalance;
            if (market->getBalance().assetB != 0 && drawdown > preventDrawdownCoeff) {
                return {
                    .reset = true
                };
            }
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
            checkPointBalance = market->getBalance().asAssetA();
            return {
                .order = risk
            };
        } else {
            checkPointBalance = market->getBalance().asAssetA();
            return {
                .order = -risk
            };
        }
        return {};
    }

    bool HawkesProcessStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 6) {
            return false;
        }

        const int* atrPeriod = std::get_if<int>(&paramSet[0]);
        const int* normRangePeriod = std::get_if<int>(&paramSet[1]);
        const int* normalRangeSmoothPeriod = std::get_if<int>(&paramSet[2]);
        const double* risk = std::get_if<double>(&paramSet[3]);
        const int* preventDrawdown = std::get_if<int>(&paramSet[4]);
        const double* preventDrawdownCoeff = std::get_if<double>(&paramSet[5]);
        if (atrPeriod == nullptr ||
            normRangePeriod == nullptr ||
            preventDrawdown == nullptr ||
            normalRangeSmoothPeriod == nullptr ||
            risk == nullptr ||
            preventDrawdownCoeff == nullptr
        ) {
            return false;
        }
        if (*atrPeriod < 1 || *normRangePeriod < 1 || *normalRangeSmoothPeriod < 1) {
            return false;
        }
        if (*risk < 0) {
            return false;
        }
        if (*preventDrawdown < 0 || *preventDrawdown > 1 || *preventDrawdownCoeff < 0 || *preventDrawdownCoeff > 1) {
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
