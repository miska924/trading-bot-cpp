#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"
#include "strategies/averaging_strategy.h"
#include "strategies/sma_bounce_strategy.h"
#include "strategies/donchain_strategy.h"
#include "traders/simple_trader.h"


using namespace TradingBot;


const double EPS = 1e-5;
const std::string testDataFileName = "../../../../test_data/btcusdt_15m_3y.csv";
const std::vector<Candle> candles = readCSVFile(testDataFileName);

const std::string gazpTestDataFileName = "../../../../test_data/gazp_1h_3y.csv";
const std::vector<Candle> gazpCandles = readCSVFile(gazpTestDataFileName);


TEST(AutoFitStrategyTest, TestAutoFitStrategy) {
    BacktestMarket market(candles);
    AutoFitStrategy<MACDHoldSlowStrategy> strategy(
        {1000, 0, 1000, 100, 0, 1.0},
        {1, 1},
        {1000, 1000}
    );
    SimpleTrader(&strategy, &market).run();

    plot("TestAutoFitMACDHoldSlowStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        144.55431314333848
    );
}

TEST(AutoFitStrategyTest, TestAutoFitMACDHoldFixedStrategy) {
    BacktestMarket market(candles);
    AutoFitStrategy<MACDHoldFixedStrategy> strategy(
        {1000, 0, 1000, 1000, 0, 1.0},
        {1, 1, 1},
        {1000, 1000, 1000}
    );
    SimpleTrader(&strategy, &market).run();

    plot("TestAutoFitMACDHoldFixedStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        110.30551980797605
    );
}

TEST(AutoFitStrategyTest, TestAutoFitStrategyForceStop) {
    BacktestMarket market(candles);
    AutoFitStrategy<MACDHoldSlowStrategy> strategy(
        {1000, 0, 1000, 100, 1, 1.0},
        {1, 1},
        {1000, 1000}
    );
    SimpleTrader(&strategy, &market).run();

    plot("TestAutoFitMACDHoldSlowStrategyForceStop.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        131.32287169509021
    );
}

TEST(AutoFitStrategyTest, TestAutoFitAveragingStrategy) {
    BacktestMarket market(candles);
    AutoFitStrategy<AveragingStrategy> strategy(
        {10000, 0, 1000, 1000, 0, 1.0},
        {1000, 40, 1000, 3.0, 0.1},
        {1000, 60, 1000, 40.0, 0.1}
    );
    SimpleTrader(&strategy, &market).run();

    plot("TestAutoFitAveragingStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        96.57225940480653
    );
}

TEST(AutoFitStrategyTest, TestAutoFitGAZP) {
    BacktestMarket market(gazpCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<MACDHoldFixedStrategy> strategy(
        {5000, 0, 1000, 1000, 0, 1.0},
        {1, 1, 1},
        {100, 1000, 1000}
    );
    SimpleTrader(&strategy, &market).run();

    plot("TestAutoFitGAZP.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        2683.561363456884
    );
}

TEST(AutoFitStrategyTest, TestAutoFitSMABounceGAZP) {
    BacktestMarket market(
        gazpCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<SMABounceStrategy> strategy(
        {1000, 0, 1000, 1000, 1, 1.0},
        {90, 100, 1.0, 0.5},
        {90, 100, 2.0, 5.0}
    );
    SimpleTrader(&strategy, &market).run();

    plot(
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
    BacktestMarket market(
        gazpCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<DonchainStrategy> strategy(
        {5000, 0, 1000, 700, 1, 1.05},
        { 10},
        { 700 }
    );
    SimpleTrader(&strategy, &market).run();

    plot(
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
    BacktestMarket market(
        gazpCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<DonchainLastLoserStrategy> strategy(
        {5000, 0, 1000, 700, 1, 1.00},
        { 10},
        { 700 }
    );
    SimpleTrader(&strategy, &market).run();

    plot(
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
