#include "strategies/sma_bounce_strategy.h"

#include <optional>

#include "helpers/vector_view.h"


namespace TradingBot {

    SMABounceStrategy::SMABounceStrategy(
        Market* market, int period, int atrPeriod, double gapOutCoeff, double gap
    ) :
        period(period),
        atrPeriod(atrPeriod),
        gapOutCoeff(gapOutCoeff),
        gap(gap),
        smaFeature(period),
        atrFeature(atrPeriod)
    {
        paramSet = {period, atrPeriod, gapOutCoeff, gap};
        assert(checkParamSet(paramSet));
        this->market = market;
    }

    SMABounceStrategy::SMABounceStrategy(
        Market* market,
        const ParamSet& paramSet
    ) : SMABounceStrategy(
        market,
        std::get<int>(paramSet[0]),
        std::get<int>(paramSet[1]),
        std::get<double>(paramSet[2]),
        std::get<double>(paramSet[3])
    ) {}

    std::vector<std::vector<std::pair<time_t, double> > > SMABounceStrategy::getPlots() {
        return {ups, downs};
    }

    bool SMABounceStrategy::checkParamSet(const ParamSet& paramSet) const {
        if (paramSet.size() != 4) {
            return false;
        }

        const int* period = std::get_if<int>(&paramSet[0]);
        const int* atrPeriod = std::get_if<int>(&paramSet[1]);
        const double* gapOutCoeff = std::get_if<double>(&paramSet[2]);
        const double* gap = std::get_if<double>(&paramSet[3]);
        if (period == nullptr || gap == nullptr || atrPeriod == nullptr || gapOutCoeff == nullptr) {
            return false;
        }
        if (*period < 1 || *gap < 0 || *atrPeriod < 1 || *gapOutCoeff < 1.0) {
            return false;
        }
        return true;
    }

    void SMABounceStrategy::step() {
        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() <= period || candles.size() < atrPeriod) {
            return;
        }
        double sma = smaFeature(candles, true);
        double atr = atrFeature(candles, true);

        double price = candles.back().close;
        double fee = market->getFee() * price;

        double up = sma + atr * gap;
        double down = sma - atr * gap;
        double up2 = sma + atr * gap * gapOutCoeff;
        double down2 = sma - atr * gap * gapOutCoeff;

        ups.push_back({candles.back().time, up});
        downs.push_back({candles.back().time, down});


        if (up < price) {
            side = OrderSide::BUY;
            // std::cerr << "side BUY" << std::endl;
        } else if (price < down) {
            side = OrderSide::SELL;
            // std::cerr << "side SELL" << std::endl;
        }

        if (side == OrderSide::RESET) {
            return;
        }

        bool newInGap = false;
        if (down <= price && price <= up) {
            newInGap = true;
        }

        // if (waiting && (waitLeft-- <= 0 || (
        //     false
        //     // || (price < down && market->getBalance().assetB > 0) ||
        //     // (price > up && market->getBalance().assetB < 0)
        // ))) {
        double position = market->getBalance().assetB;
        if (waiting && (
            (position > 0 && (price < down2 || price > up2)) ||
            (position < 0 && (price > up2 || price < down2))
        )) {
            waiting = false;
            market->order({.side = OrderSide::RESET});
        } 
        if (!waiting && side != OrderSide::RESET && !inGap && newInGap) {
            waiting = true;
            market->order({.side = side, .amount = 1});
        }

        inGap = newInGap;
    }

} // namespace TradingBot
