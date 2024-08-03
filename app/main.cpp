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


int main() {
    TradingBot::TinkoffMarket market(
        2 * 365 * 24,
        TradingBot::CandleTimeDelta::CANDLE_1_HOUR,
        "GAZP",
        2
    );
    std::cerr << "market is set" << std::endl;
    Helpers::VectorView<TradingBot::Candle> candles = market.getCandles();
    std::cerr << "candles: " << candles.size() << std::endl;
    writeCSVFile("../test_data/gazp_1h_3y.csv", candles);
    return 0;
}
