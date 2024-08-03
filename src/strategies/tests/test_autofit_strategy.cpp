#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"
#include "strategies/averaging_strategy.h"
#include "strategies/sma_bounce_strategy.h"
#include "strategies/donchain_strategy.h"


const double EPS = 1e-5;
const std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
const std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

const std::string gazpTestDataFileName = "../../../../test_data/gazp_1h_3y.csv";
const std::vector<TradingBot::Candle> gazpCandles = TradingBot::readCSVFile(gazpTestDataFileName);


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
        144.55431314333848
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
        110.30551980797605
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
        131.32287169509021
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
        96.57225940480653
    );
}

TEST(AutoFitStrategyTest, TestAutoFitGAZP) {
    TradingBot::BacktestMarket market(gazpCandles, true, false, 0.003, {.assetA = 2000});
    TradingBot::AutoFitStrategy<TradingBot::MACDHoldFixedStrategy> strategy(
        &market,
        {5000, 0, 1000, 1000, 0, 1.0},
        {1, 1, 1},
        {100, 1000, 1000}
    );
    strategy.run();

    TradingBot::plot("TestAutoFitGAZP.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        2683.561363456884
    );
}

TEST(AutoFitStrategyTest, TestAutoFitSMABounceGAZP) {
    TradingBot::BacktestMarket market(
        gazpCandles, true, false, 0.003, {.assetA = 2000});
    TradingBot::AutoFitStrategy<TradingBot::SMABounceStrategy> strategy(
        &market,
        {1000, 0, 1000, 1000, 1, 1.0},
        {90, 100, 1.0, 0.5},
        {90, 100, 2.0, 5.0}
    );
    strategy.run();

    TradingBot::plot(
        "TestAutoFitSMABounceGAZP.png",
        market.getCandles(),
        market.getOrderHistory(),
        market.getBalanceHistory(),
        strategy.getPlots()
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        4731.3559663538499
    );
}

TEST(AutoFitStrategyTest, TestAutoFitDonchainGAZP) {
    TradingBot::BacktestMarket market(
        gazpCandles, true, false, 0.003, {.assetA = 2000});
    TradingBot::AutoFitStrategy<TradingBot::DonchainStrategy> strategy(
        &market,
        {5000, 0, 1000, 700, 1, 1.05},
        { 10},
        { 700 }
    );
    strategy.run();

    TradingBot::plot(
        "TestAutoFitDonchainGAZP.png",
        market.getCandles(),
        market.getOrderHistory(),
        market.getBalanceHistory(),
        strategy.getPlots()
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        3524.3113513751705
    );
}

TEST(AutoFitStrategyTest, TestAutoFitDonchainLastLoserGAZP) {
    TradingBot::BacktestMarket market(
        gazpCandles, true, false, 0.003, {.assetA = 2000});
    TradingBot::AutoFitStrategy<TradingBot::DonchainLastLoserStrategy> strategy(
        &market,
        {5000, 0, 1000, 700, 1, 1.00},
        { 10},
        { 700 }
    );
    strategy.run();

    TradingBot::plot(
        "TestAutoFitDonchainLastLoserGAZP.png",
        market.getCandles(),
        market.getOrderHistory(),
        market.getBalanceHistory(),
        strategy.getPlots()
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        3957.647588279322
    );
}
