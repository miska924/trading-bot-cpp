#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"


const double EPS = 1e-5;
const std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
const std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);


TEST(AutoFitStrategyTest, TestAutoFitStrategy) {
    TradingBot::BacktestMarket market(candles);
    TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(
        &market,
        {1000, 1000, 100, 0, TradingBot::Balance().asAssetA()},
        {1, 1},
        {1000, 1000}
    );
    strategy.run();

    TradingBot::plot("TestAutoFitStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        121.47887400111658
    );
}

TEST(AutoFitStrategyTest, TestAutoFitStrategyForceStop) {
    TradingBot::BacktestMarket market(candles);
    TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(
        &market,
        {1000, 1000, 100, 1, TradingBot::Balance().asAssetA()},
        {1, 1},
        {1000, 1000}
    );
    strategy.run();

    TradingBot::plot("TestAutoFitStrategyForceStop.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        164.84977961015281
    );
}