#include <iostream>
#include <vector>

#include "markets/tinkoff_market.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"


int main() {
    TradingBot::TinkoffMarket market(3000, false);
    TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(
        &market,
        {1000, 0, 1000, 100, 0, TradingBot::Balance().asAssetA()},
        {1, 1},
        {1000, 1000}
    );
    strategy.run();
    return 0;
}
