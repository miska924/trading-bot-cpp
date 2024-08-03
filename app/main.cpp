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
#include "helpers/vector_view.h"
#include "markets/market.h"


using namespace TradingBot;

int main() {

    TinkoffMarket market(
        10000,
        CandleTimeDelta::CANDLE_1_MIN,
        "GAZP",
        2
    );

    AutoFitStrategy<AveragingStrategy> strategy(
        {10000, 0, 1, 1, 0, 1.0},
        {300, 5000, 1000, 1.5, 1.0},
        {300, 5000, 1000, 1.5, 1.0}
    );

    SimpleTrader(&strategy, &market).run();

    return 0;
}
