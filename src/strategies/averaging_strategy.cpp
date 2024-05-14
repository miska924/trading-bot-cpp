#include "strategies/averaging_strategy.h"


namespace TradingBot {

    AveragingStrategy::AveragingStrategy(
        Market* market, const ParamSet& paramSet
    ) {
        assert(checkParamSet(paramSet));

        this->market = market;

        this->paramSet = paramSet;
        period = std::get<int>(paramSet[0]);
        waitCandles = std::get<int>(paramSet[1]);
        coeff = std::get<double>(paramSet[2]);
        risk = std::get<double>(paramSet[3]);

        atr = ATRFeature(period);
    }

    AveragingStrategy::AveragingStrategy(
        Market* market,
        int period,
        int waitCandles,
        double coeff,
        double risk
    ) : period(period), waitCandles(waitCandles), coeff(coeff), risk(risk) {
        paramSet = {period, waitCandles, coeff, risk};
        assert(checkParamSet(paramSet));
        this->market = market;
        atr = ATRFeature(period);
    }

    bool AveragingStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 4) {
            return false;
        }

        const int* period = std::get_if<int>(&paramSet[0]);
        const int* waitCandles = std::get_if<int>(&paramSet[1]);
        const double* coeff = std::get_if<double>(&paramSet[2]);
        const double* risk = std::get_if<double>(&paramSet[3]);
        if (period == nullptr || coeff == nullptr || waitCandles == nullptr || risk == nullptr) {
            return false;
        }
        if (*period < 1 || *waitCandles < 1 || *coeff < 0 || *risk < 0) {
            return false;
        }

        return true;
    }

    void AveragingStrategy::step() {
        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() <= period) {
            return;
        }

        if (market->time() >= waitUntill && inCombo) {
            market->order({.side = OrderSide::RESET});
            inCombo = false;
        }

        if (atrValue && !noUpdateCount) {
            atrValue = atr(candles, false);
        } else {
            atrValue = atr(candles);
            noUpdateCount = period;
        }

        if (inCombo) {
            if (lastOrder.side == OrderSide::BUY) {
                if (lastOrder.price - candles.back().close >= savedAtr * coeff) {
                    if (position == positionsLimit) {
                        market->order({.side = OrderSide::RESET});
                        inCombo = false;
                        return;
                    }
                    noLoss = (noLoss * position + candles.back().close) / (position + 1);
                    ++position;
                    lastOrder = {.side = OrderSide::BUY, .amount = risk, .price = candles.back().close};
                    market->order(lastOrder);
                } else if (candles.back().close - noLoss >= savedAtr * coeff) {
                    market->order({.side = OrderSide::RESET});
                    inCombo = false;
                }
            } else if (lastOrder.side == OrderSide::SELL) {
                if (candles.back().close - lastOrder.price >= savedAtr * coeff) {
                    if (position == positionsLimit) {
                        market->order({.side = OrderSide::RESET});
                        inCombo = false;
                        return;
                    }
                    noLoss = (noLoss * position + candles.back().close) / (position + 1);
                    ++position;
                    lastOrder = {.side = OrderSide::SELL, .amount = risk, .price = candles.back().close};
                    market->order(lastOrder);
                } else if (noLoss - candles.back().close >= savedAtr * coeff) {
                    market->order({.side = OrderSide::RESET});
                    inCombo = false;
                }
            }
            return;
        }

        if (random() % 2) {
            lastOrder = {.side = OrderSide::BUY, .amount = risk, .price = candles.back().close};
            market->order(lastOrder);
            waitUntill = market->time() + waitCandles * market->getCandleTimeDelta();
        } else {
            lastOrder = {.side = OrderSide::SELL, .amount = risk, .price = candles.back().close};
            market->order(lastOrder) + waitCandles * market->getCandleTimeDelta();
            waitUntill = market->time() + waitCandles * market->getCandleTimeDelta();
        }
        savedAtr = atrValue;
        noLoss = candles.back().close;
        position = 1;
        inCombo = true;
    }

} // namespace TradingBot
