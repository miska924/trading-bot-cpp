#include <iostream>
#include <vector>

#include "markets/tinkoff_market.h"
#include "markets/backtest_market.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"
#include "strategies/hawks_process_strategy.h"
#include "strategies/averaging_strategy.h"
#include "strategies/dummy_strategy.h"
#include "plotting/plotting.h"
#include "helpers/date_time.h"


int main() {
    TradingBot::TinkoffMarket market(
        400,
        TradingBot::CandleTimeDelta::CANDLE_1_MIN,
        "GAZP",
        2
    );
    TradingBot::DummyStrategy strategy(&market);
    strategy.run();
    // test();
    return 0;
}
