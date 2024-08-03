#include "strategies/averaging_strategy.h"


namespace TradingBot {

    AveragingStrategy::AveragingStrategy(const ParamSet& paramSet) {
        assert(checkParamSet(paramSet));

        this->paramSet = paramSet;
        atrPeriod = std::get<int>(paramSet[0]);
        positionSidePeriod = std::get<int>(paramSet[1]);
        waitCandles = std::get<int>(paramSet[2]);
        coeff = std::get<double>(paramSet[3]);
        risk = std::get<double>(paramSet[4]);

        atr = ATRFeature(atrPeriod);
    }

    AveragingStrategy::AveragingStrategy(
        int atrPeriod,
        int positionSidePeriod,
        int waitCandles,
        double coeff,
        double risk
    ) :
        atrPeriod(atrPeriod),
        positionSidePeriod(positionSidePeriod),
        waitCandles(waitCandles),
        coeff(coeff),
        risk(risk) 
    {
        paramSet = {atrPeriod, positionSidePeriod, waitCandles, coeff, risk};
        assert(checkParamSet(paramSet));
        atr = ATRFeature(atrPeriod);
    }

    bool AveragingStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 5) {
            return false;
        }

        const int* atrPeriod = std::get_if<int>(&paramSet[0]);
        const int* positionSidePeriod = std::get_if<int>(&paramSet[1]);
        const int* waitCandles = std::get_if<int>(&paramSet[2]);
        const double* coeff = std::get_if<double>(&paramSet[3]);
        const double* risk = std::get_if<double>(&paramSet[4]);
        if (atrPeriod == nullptr || coeff == nullptr || waitCandles == nullptr || risk == nullptr || positionSidePeriod == nullptr) {
            return false;
        }
        if (*atrPeriod < 1 || *waitCandles < 1 || *coeff < 0 || *risk < 0 || *positionSidePeriod < 1) {
            return false;
        }

        return true;
    }

    Signal AveragingStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() <= std::max(atrPeriod, positionSidePeriod)) {
            return {};
        }

        bool reset = false;
        if (market->time() >= waitUntill && inCombo) {
            inCombo = false;
            reset = true;
        }

        if (atrValue && noUpdateCount) {
            atrValue = atr(candles, true);
            --noUpdateCount;
        } else {
            atrValue = atr(candles);
            noUpdateCount = atrPeriod;
        }

        if (inCombo) {
            if (lastOrder.side == OrderSide::BUY) {
                if (lastOrder.price - candles.back().close >= savedAtr * coeff) {
                    if (position == positionsLimit) {
                        inCombo = false;
                        return {
                            .reset = true
                        };
                    }
                    positionsSum = positionsSum + candles.back().close;
                    ++position;
                    lastOrder = {.side = OrderSide::BUY, .amount = risk, .price = candles.back().close};
                    return {
                        .reset = reset,
                        .order = risk
                    };
                } else if (candles.back().close - positionsSum / position >= savedAtr * coeff) {
                    inCombo = false;
                    return {
                        .reset = true
                    };
                }
            } else if (lastOrder.side == OrderSide::SELL) {
                if (candles.back().close - lastOrder.price >= savedAtr * coeff) {
                    if (position == positionsLimit) {
                        inCombo = false;
                        return {
                            .reset = true
                        };
                    }
                    positionsSum = positionsSum + candles.back().close;
                    ++position;
                    lastOrder = {.side = OrderSide::SELL, .amount = risk, .price = candles.back().close};
                    return {
                        .reset = reset,
                        .order = -risk
                    };
                } else if (positionsSum / position - candles.back().close >= savedAtr * coeff) {
                    inCombo = false;
                    return {
                        .reset = true
                    };
                }
            }
            return {
                .reset = reset
            };
        }

        Signal signal = {
            .reset = reset
        };
        if (candles.back().close > candles[candles.size() - positionSidePeriod].close) {
            lastOrder = {.side = OrderSide::BUY, .amount = risk, .price = candles.back().close};
            signal.order = risk;
            waitUntill = market->time() + waitCandles * market->getCandleTimeDelta();
        } else {
            lastOrder = {.side = OrderSide::SELL, .amount = risk, .price = candles.back().close};
            signal.order = -risk;
            waitUntill = market->time() + waitCandles * market->getCandleTimeDelta();
        }
        savedAtr = atrValue;
        positionsSum = candles.back().close;
        position = 1;
        inCombo = true;
        return signal;
    }

} // namespace TradingBot
