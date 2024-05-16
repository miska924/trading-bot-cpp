#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"
#include "strategies/averaging_strategy.h"


const double EPS = 1e-5;
const std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
const std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);


TEST(AutoFitStrategyTest, TestAutoFitStrategy) {
    TradingBot::BacktestMarket market(candles);
    TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(
        &market,
        {1000, 0, 1000, 100, 0, 1.0},
        {1, 1},
        {1000, 1000}
    );
    strategy.run();

    TradingBot::plot("TestAutoFitMACDHoldSlowStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        154.36272866519681
    );
}

TEST(AutoFitStrategyTest, TestAutoFitMACDHoldFixedStrategy) {
    TradingBot::BacktestMarket market(candles);
    TradingBot::AutoFitStrategy<TradingBot::MACDHoldFixedStrategy> strategy(
        &market,
        {1000, 0, 1000, 1000, 0, 1.0},
        {1, 1, 1},
        {1000, 1000, 1000}
    );
    strategy.run();

    TradingBot::plot("TestAutoFitMACDHoldFixedStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        156.97329832407993
    );
}

TEST(AutoFitStrategyTest, TestAutoFitStrategyForceStop) {
    TradingBot::BacktestMarket market(candles);
    TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(
        &market,
        {1000, 0, 1000, 100, 1, 1.0},
        {1, 1},
        {1000, 1000}
    );
    strategy.run();

    TradingBot::plot("TestAutoFitMACDHoldSlowStrategyForceStop.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        124.1775374311995
    );
}

TEST(AutoFitStrategyTest, TestAutoFitAveragingStrategy) {
    TradingBot::BacktestMarket market(candles);
    TradingBot::AutoFitStrategy<TradingBot::AveragingStrategy> strategy(
        &market,
        {10000, 0, 1000, 1000, 0, 1.0},
        {1000, 40, 1000, 3.0, 0.1},
        {1000, 60, 1000, 40.0, 0.1}
    );
    strategy.run();

    TradingBot::plot("TestAutoFitAveragingStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        124.16714631107786
    );
}
