#include "traders/trader.h"

#include <optional>

namespace TradingBot {

    Trader::Trader(Strategy* strategy, Market* market) :
        strategy(strategy),
        marketInfo(market),
        market(market)
    {
        strategy->attachMarketInfo(&marketInfo);
    }

    void Trader::run() {
        do {
            step(market->update());
        } while (!market->finished());
    }

} // namespace TradingBot
