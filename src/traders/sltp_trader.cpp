#include "traders/sltp_trader.h"

#include <optional>


namespace TradingBot {

    SLTPTrader::SLTPTrader(
        Strategy* strategy, Market* market, double stopLoss, double takeProfit, double risk
    ) : Trader(strategy, market), stopLoss(stopLoss), takeProfit(takeProfit), risk(risk) {};
    
    Order SLTPTrader::setSLTP(Order order) {
        auto candles = market->getCandles();
        if (candles.size() < DEFAULT_ATR_PERIOD) {
            return {};
        }

        double atr = atrFeature(candles, false);

        if (order.side == OrderSide::BUY) {
            order.stopLoss = candles.back().close - atr * stopLoss;
            order.takeProfit = candles.back().close + atr * takeProfit;
            // order.stopLoss = candles.back().close * (1.0 - stopLoss);
            // order.takeProfit = candles.back().close * (1.0 + takeProfit);
            order.amount = risk * candles.back().close / (candles.back().close - order.stopLoss);
        } else if (order.side == OrderSide::SELL) {
            order.stopLoss = candles.back().close + atr * stopLoss;
            order.takeProfit = candles.back().close - atr * takeProfit;
            // order.stopLoss = candles.back().close * (1.0 + stopLoss);
            // order.takeProfit = candles.back().close * (1.0 - takeProfit);
            order.amount = risk * candles.back().close / (order.stopLoss - candles.back().close);
        }

        return order;
    }

    void SLTPTrader::step(bool newCandle) {
        Signal signal = strategy->step(newCandle);

        // if (signal.reset) {
        //     market->order({.side = OrderSide::RESET});
        // }

        if (signal.order == 0) {
            return;
        }

        Order order;
        if (market->getBalance().assetB == 0 && signal.order > 0.0) {
            order = {.side = OrderSide::BUY, .amount = signal.order};
        } else if (market->getBalance().assetB == 0 && signal.order < 0.0) {
            order = {.side = OrderSide::SELL, .amount = -signal.order};
        } else {
            return;
        }

        order = setSLTP(order);
        market->order(order);
    }

} // namespace TradingBot