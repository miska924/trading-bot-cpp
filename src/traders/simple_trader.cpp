#include "traders/simple_trader.h"

#include <optional>


namespace TradingBot {

    SimpleTrader::SimpleTrader(Strategy* strategy, Market* market) : Trader(strategy, market) {};
    
    void SimpleTrader::step(bool newCandle) {
        Signal signal = strategy->step(newCandle);

        if (signal.reset) {
            market->order({.side = OrderSide::RESET});
        }

        if (signal.order == 0) {
            return;
        }

        if (signal.order > 0.0) {
            market->order({.side = OrderSide::BUY, .amount = signal.order});
        } else if (signal.order < 0.0) {
            market->order({.side = OrderSide::SELL, .amount = -signal.order});
        }
    }

} // namespace TradingBot