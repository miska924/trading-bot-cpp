#include "hawks_process_strategy.h"


namespace TradingBot {

    HawksProcessStrategy::HawksProcessStrategy(
        Market* market,
        const ParamSet& paramSet
    ) {
        assert(checkParamSet(paramSet));
        this->market = market;
        atrPeriod = std::get<int>(paramSet[0]);
        normRangePeriod = std::get<int>(paramSet[1]);
        normalRangeSmoothPeriod = std::get<int>(paramSet[2]);
        risk = std::get<double>(paramSet[3]);
        preventDrawdown = std::get<int>(paramSet[4]);
        preventDrawdownCoeff = std::get<double>(paramSet[5]);
        atr = ATRFeature(atrPeriod, true);
        // checkPointBalance = market->getBalance().asAssetA();
    }

    HawksProcessStrategy::HawksProcessStrategy(
        Market* market,
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
        this->market = market;
        atr = ATRFeature(atrPeriod, true);
        // checkPointBalance = market->getBalance().asAssetA();
    }

    void HawksProcessStrategy::step() {
        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() < atrPeriod) {
            return;
        }

        // checkPointBalance = std::max(checkPointBalance, market->getBalance().asAssetA());
        if (preventDrawdown) {
            double drawdown = (checkPointBalance - market->getBalance().asAssetA()) / checkPointBalance;
            if (market->getBalance().assetB != 0 && drawdown > preventDrawdownCoeff) {
                market->order({.side = OrderSide::RESET});
                return;
            }
        }
            
        double atrValue = atr(candles, true);
        double logDiff = (std::log1p(candles.back().high) - std::log1p(candles.back().low));
        double normRangeValue = logDiff / atrValue;
        updateNormRange(normRangeValue, candles.size() - 1);

        if (normRange.size() < normRangePeriod) {
            return;
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
            return;
        }

        if (lastDownCrossIndex > lastUpCrossIndex) {
            if (market->getBalance().assetB != 0) {
                market->order({.side = OrderSide::RESET});
            }
            return;
        }

        if (market->getBalance().assetB != 0) {
            return;
        }

        if (candles[lastDownCrossIndex].close < candles[lastUpCrossIndex].close) {
            market->order({.side = OrderSide::BUY, .amount = risk});
            checkPointBalance = market->getBalance().asAssetA();
        } else {
            market->order({.side = OrderSide::SELL, .amount = risk});
            checkPointBalance = market->getBalance().asAssetA();
        }
    }

    bool HawksProcessStrategy::checkParamSet(const ParamSet& paramSet) const {
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
        if (*preventDrawdown == 0 && *preventDrawdownCoeff != 0) {
            return false;
        }
        return true;
    }

    void HawksProcessStrategy::updateNormRange(double value, int index) {
        normRangeSorted.insert({value, index});
        normRangeSorted.erase(normRange.front());
        normRange.push_back({value, index});
        if (normRange.size() > normRangePeriod) {
            normRange.pop_front();
        }
    }

} // namespace TradingBot
