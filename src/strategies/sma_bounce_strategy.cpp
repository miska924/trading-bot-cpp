#include "strategies/sma_bounce_strategy.h"

#include <optional>

#include "helpers/vector_view.h"


namespace TradingBot {

    SMABounceStrategy::SMABounceStrategy(
        int period, int atrPeriod, double gapOutCoeff, double gap
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
    }

    SMABounceStrategy::SMABounceStrategy(
        const ParamSet& paramSet
    ) : SMABounceStrategy(
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

    Signal SMABounceStrategy::step(bool newCandle) {
        if (!newCandle) {
            return {};
        }

        Helpers::VectorView<Candle> candles = market->getCandles();
        if (candles.size() <= period || candles.size() < atrPeriod) {
            return {};
        }
        double sma = smaFeature(candles, true);
        double atr = atrFeature(candles, true);

        double price = candles.back().close;
        double fee = market->getFee() * price;

        double up = sma + atr * gap;
        double down = sma - atr * gap;
        double up2 = sma + atr * gap * gapOutCoeff;
        double down2 = sma - atr * gap * gapOutCoeff;

        if (savePlots) {
            ups.push_back({candles.back().time, up});
            downs.push_back({candles.back().time, down});
        }


        if (up < price) {
            side = OrderSide::BUY;
        } else if (price < down) {
            side = OrderSide::SELL;
        }

        if (side == OrderSide::RESET) {
            return {};
        }

        bool newInGap = false;
        if (down <= price && price <= up) {
            newInGap = true;
        }

        bool reset = false;
        double order = 0.0;
        double position = market->getBalance().assetB;
        if (waiting && (
            (position > 0 && (price < down2 || price > up2)) ||
            (position < 0 && (price > up2 || price < down2))
        )) {
            waiting = false;
            reset = true;
        } 
        if (!waiting && side != OrderSide::RESET && !inGap && newInGap) {
            waiting = true;
            order = side == OrderSide::BUY ? 1 : -1;
        }

        inGap = newInGap;

        return {
            .reset = reset,
            .order = order,
        };
    }

} // namespace TradingBot
