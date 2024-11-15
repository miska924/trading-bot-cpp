#include <gtest/gtest.h>

#include "markets/backtest_market.h"
#include "plotting/plotting.h"
#include "strategies/ema_crossover_strategy.h"
#include "strategies/autofit_strategy.h"
#include "traders/sltp_trader.h"


using namespace TradingBot;


std::vector<Candle> btcusdt15m10dCandles = readCSVFile("../../../../test_data/btcusdt_15m_10d.csv");
std::vector<Candle> btcusdt15m3yCandles = readCSVFile("../../../../test_data/btcusdt_15m_3y.csv");
std::vector<Candle> gazp1h3yCandles = readCSVFile("../../../../test_data/gazp_1h_3y.csv");


TEST(SLTPTraderTest, TestEMACrossoverStrategyGAZP) {
    BacktestMarket market(gazp1h3yCandles, true, false, 0.0015, {.assetA = 2000});
    AutoFitStrategy<EMACrossoverStrategy> strategy(
        {5000, 0, 1000, 1000, 0, 1.00},
        {1, 100},
        {100, 1000}
    );
    strategy.enableSavingPlots();
    PercentSLTPTrader(&strategy, &market, 0.01, 0.1, 0.01).run();
    plot("TestSLPTTraderEMACrossoverStrategyGAZP.png", market.getCandles().toVector(), market.getOrderHistory(), market.getBalanceHistory(), strategy.getPlots(), false);

    EXPECT_EQ(market.getOrderHistory().size(), 69);
    EXPECT_EQ(market.getBalance().asAssetA(), 20357.725915498944);
}
