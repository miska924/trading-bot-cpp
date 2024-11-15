#include "traders/sltp_trader.h"

#include <optional>


namespace TradingBot {

    PercentSLTrader::PercentSLTrader(
        Strategy* strategy, Market* market, double stopLoss, double risk
    ) : Trader(strategy, market), stopLoss(stopLoss), risk(risk) {};
    
    Order PercentSLTrader::setSL(Order order) {
        auto candles = market->getCandles();
        // if (candles.size() < DEFAULT_ATR_PERIOD) {
        //     return {};
        // }

        // double atr = atrFeature(candles, false);

        double close = candles.back().close;

        if (order.side == OrderSide::BUY) {
            // order.stopLoss = close - atr * stopLoss;
            order.stopLoss = close * (1.0 - stopLoss);
        } else if (order.side == OrderSide::SELL) {
            // order.stopLoss = close + atr * stopLoss;
            order.stopLoss = close * (1.0 + stopLoss);
        }
        order.amount = risk / stopLoss;

        return order;
    }

    void PercentSLTrader::step(bool newCandle) {
        Signal signal = strategy->step(newCandle);

        if (signal.reset) {
            market->order({.side = OrderSide::RESET});
        }

        Order order;
        if (market->getBalance().assetB == 0 && signal.order > 0.0) {
            order = {.side = OrderSide::BUY, .amount = signal.order};
        } else if (market->getBalance().assetB == 0 && signal.order < 0.0) {
            order = {.side = OrderSide::SELL, .amount = -signal.order};
        } else {
            return;
        }

        order = setSL(order);
        market->order(order);
    }

    DynamicPercentSLTrader::DynamicPercentSLTrader(
        Strategy* strategy, Market* market, double stopLoss, double risk
    ) : Trader(strategy, market), stopLoss(stopLoss), risk(risk) {};
    
    Order DynamicPercentSLTrader::setSL(Order order) {
        auto candles = market->getCandles();
        // if (candles.size() < DEFAULT_ATR_PERIOD) {
        //     return {};
        // }

        // double atr = atrFeature(candles, false);

        double close = candles.back().close;

        if (order.side == OrderSide::BUY) {
            // order.stopLoss = close - atr * stopLoss;
            order.stopLoss = close * (1.0 - stopLoss);
        } else if (order.side == OrderSide::SELL) {
            // order.stopLoss = close + atr * stopLoss;
            order.stopLoss = close * (1.0 + stopLoss);
        }
        order.amount = risk / stopLoss;

        return order;
    }

    void DynamicPercentSLTrader::step(bool newCandle) {
        Signal signal = strategy->step(newCandle);

        if (signal.reset) {
            market->order({.side = OrderSide::RESET});
        }

        {
            Order original = market->getLastOrder();
            Order order = original;
            order = setSL(order);
            if (order.stopLoss > original.stopLoss && order.side == OrderSide::BUY) {
                std::cerr << "buy  " << original.stopLoss << " -> " << order.stopLoss << std::endl;
                market->ModifyStopLossTakeProfit(order.stopLoss, original.takeProfit);
            } else if (order.stopLoss < original.stopLoss && order.side == OrderSide::SELL) {
                std::cerr << "sell " << original.stopLoss << " -> " << order.stopLoss << std::endl;
                market->ModifyStopLossTakeProfit(order.stopLoss, original.takeProfit);
            }
        }

        Order order;
        if (market->getBalance().assetB == 0 && signal.order > 0.0) {
            order = {.side = OrderSide::BUY, .amount = signal.order};
        } else if (market->getBalance().assetB == 0 && signal.order < 0.0) {
            order = {.side = OrderSide::SELL, .amount = -signal.order};
        } else {
            return;
        }

        order = setSL(order);
        market->order(order);
    }

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

    PercentSLTPTrader::PercentSLTPTrader(
        Strategy* strategy, Market* market, double stopLoss, double takeProfit, double risk
    ) : Trader(strategy, market), stopLoss(stopLoss), takeProfit(takeProfit), risk(risk) {};
    
    Order PercentSLTPTrader::setSLTP(Order order) {
        auto candles = market->getCandles();
        if (candles.size() < DEFAULT_ATR_PERIOD) {
            return {};
        }

        double atr = atrFeature(candles, false);

        if (order.side == OrderSide::BUY) {
            // order.stopLoss = candles.back().close - atr * stopLoss;
            // order.takeProfit = candles.back().close + atr * takeProfit;
            order.stopLoss = candles.back().close * (1.0 - stopLoss);
            order.takeProfit = candles.back().close * (1.0 + takeProfit);
            order.amount = risk * candles.back().close / (candles.back().close - order.stopLoss);
        } else if (order.side == OrderSide::SELL) {
            // order.stopLoss = candles.back().close + atr * stopLoss;
            // order.takeProfit = candles.back().close - atr * takeProfit;
            order.stopLoss = candles.back().close * (1.0 + stopLoss);
            order.takeProfit = candles.back().close * (1.0 - takeProfit);
            order.amount = risk * candles.back().close / (order.stopLoss - candles.back().close);
        }

        return order;
    }

    void PercentSLTPTrader::step(bool newCandle) {
        Signal signal = strategy->step(newCandle);

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