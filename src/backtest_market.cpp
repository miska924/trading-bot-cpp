#include "markets/backtest_market.h"


namespace trading_bot {

time_t BacktestMarket::time() const {
    return marketTime;
}

} // trading_bot
