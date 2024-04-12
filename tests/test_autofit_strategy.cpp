#include <gtest/gtest.h>

#include "strategies/autofit_strategy.h"
#include "strategies/macd_strategy.h"
#include "markets/backtest_market.h"
#include "plotting/plotting.h"


const double EPS = 1e-5;

TEST(AutoFitStrategyTest, TestAutoFitStrategy) {
    std::string testDataFileName = "../../test_data/btcusdt_15m_3y.csv";
    std::vector<TradingBot::Candle> candles = TradingBot::readCSVFile(testDataFileName);

    TradingBot::BacktestMarket market(candles);
    TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(&market, 10000, 1000, 100, 1);
    strategy.run();

    TradingBot::plot("TestAutoFitStrategy.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());

    // TODO: debug different balance between runs
    // EXPECT_EQ(
    //     market.getBalance().asAssetA(), 0.0
    // );

    // EXPECT_LE(
    //     std::abs(market.getBalance().asAssetA() - 104.32291561302941),
    //     EPS
    // );
}
