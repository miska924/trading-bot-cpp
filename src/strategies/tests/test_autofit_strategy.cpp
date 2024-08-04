#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"
#include "strategies/averaging_strategy.h"
#include "strategies/sma_bounce_strategy.h"
#include "strategies/donchain_strategy.h"
#include "strategies/hawkes_process_strategy.h"
#include "traders/simple_trader.h"
#include "helpers/vector_view.h"


using namespace TradingBot;
using namespace Helpers;


std::vector<Candle> btcusdt15m3yCandles = readCSVFile("../../../../test_data/btcusdt_15m_3y.csv");
std::vector<Candle> gazp1h3yCandles = readCSVFile("../../../../test_data/gazp_1h_3y.csv");
std::vector<Candle> gazp1MCandles = readCSVFile("../../../../test_data/gazp_1m_1M.csv");


TEST(AutoFitStrategyTest, TestAutoFitStrategy) {
    BacktestMarket market(btcusdt15m3yCandles);
    AutoFitStrategy<MACDHoldSlowStrategy> strategy(
        {1000, 0, 1000, 100, 0, 1.0},
        {1, 1},
        {1000, 1000}
    );
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();

    plot("TestAutoFitMACDHoldSlowStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        131.68304078794475
    );
}

TEST(AutoFitStrategyTest, TestAutoFitMACDHoldFixedStrategy) {
    BacktestMarket market(btcusdt15m3yCandles);
    AutoFitStrategy<MACDHoldFixedStrategy> strategy(
        {1000, 0, 1000, 1000, 0, 1.0},
        {1, 1, 1},
        {1000, 1000, 1000}
    );
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAutoFitMACDHoldFixedStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        47.334823719308247
    );
}

TEST(AutoFitStrategyTest, TestAutoFitStrategyForceStop) {
    BacktestMarket market(btcusdt15m3yCandles);
    AutoFitStrategy<MACDHoldSlowStrategy> strategy(
        {1000, 0, 1000, 100, 1, 1.0},
        {1, 1},
        {1000, 1000}
    );
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAutoFitMACDHoldSlowStrategyForceStop.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        86.238052594373684
    );
}

TEST(AutoFitStrategyTest, TestAutoFitAveragingStrategy) {
    BacktestMarket market(btcusdt15m3yCandles);
    AutoFitStrategy<AveragingStrategy> strategy(
        {10000, 0, 1000, 1000, 0, 1.0},
        {1000, 40, 1000, 3.0, 0.1},
        {1000, 60, 1000, 40.0, 0.1}
    );
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAutoFitAveragingStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        96.57225940480653
    );
}

TEST(AutoFitStrategyTest, TestAutoFitGAZP) {
    BacktestMarket market(gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<MACDHoldSlowStrategy> strategy(
        {1000, 0, 1000, 10000, 0, 1.00},
        {30, 300},
        {30, 300}
    );
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAutoFitGAZP.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        3006.279037446403
    );
}

TEST(AutoFitStrategyTest, TestAutoFitSMABounceGAZP) {
    BacktestMarket market(
        gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
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
        gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<DonchainStrategy> strategy(
        {5000, 0, 1000, 700, 1, 1.05},
        { 10},
        { 700 }
    );
    strategy.enableSavingPlots();
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
        gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<DonchainLastLoserStrategy> strategy(
        {5000, 0, 1000, 700, 0, 1.00},
        { 10},
        { 700 }
    );
    strategy.enableSavingPlots();
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
        3837.1050397547033
    );
}

TEST(AutoFitStrategyTest, TestAutoFitDonchainLastWinnerGAZP) {
    BacktestMarket market(
        gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<DonchainLastWinnerStrategy> strategy(
        {5000, 0, 1000, 1000, 0, 0.00},
        { 10 },
        { 700 }
    );
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot(
        "TestAutoFitDonchainLastWinnerGAZP.png",
        market.getCandles(),
        market.getOrderHistory(),
        market.getBalanceHistory(),
        strategy.getPlots()
    );

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        1017.6211712005963
    );
}

TEST(AutoFitStrategyTest, TestAutoFit1minGAZP) {
    BacktestMarket market(gazp1MCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<AveragingStrategy> strategy(
        {10000, 0, 60, 100, 0, 1.0},
        {300, 1000, 5000, 10.0, 0.1},
        {300, 1000, 5000, 10.0, 0.1}
    );
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAutoFit1minGAZP.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        1943.0441397134562
    );
}

TEST(AutoFitStrategyTest, TestAutoFitHawkesProcessGAZP) {
    BacktestMarket market(gazp1h3yCandles, true, false, 0.003, {.assetA = 2000});
    AutoFitStrategy<HawkesProcessStrategy> strategy(
        {2000, 0, 1000, 100, 0, 1.0},
        {30, 30, 4, 1.0, 1, 0.1},
        {30, 100, 4, 1.0, 1, 0.1}
    );
    strategy.enableSavingPlots();
    SimpleTrader(&strategy, &market).run();
    plot("TestAutoFitHawkesProcessGAZP.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots());

    EXPECT_EQ(
        market.getBalance().asAssetA(),
        3627.0350735827433
    );
}
