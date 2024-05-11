#include <gtest/gtest.h>

#include "markets/tinkoff_market.h"
#include "strategies/macd_strategy.h"
#include "strategies/autofit_strategy.h"
#include "plotting/plotting.h"


// TEST(TinkoffMarketTest, TestInit) {
//     TradingBot::TinkoffMarket market(1000, true);
//     TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(
//         &market,
//         {1000, 0, 1000, 100, 0, TradingBot::Balance().asAssetA()},
//         {1, 1},
//         {1000, 1000}
//     );
//     strategy.run();
//     TradingBot::Balance balance = market.getBalance();

//     EXPECT_EQ(market.getCandles().size(), 58);
//     EXPECT_EQ(balance.assetA, TradingBot::Balance().assetA);
//     EXPECT_EQ(balance.assetB, TradingBot::Balance().assetB);
// }


TEST(TinkoffMarketTest, TestDateTime) {
    EXPECT_EQ(
        TradingBot::DateTime(TradingBot::ParseDateTime("2024-05-01T01:23:34Z")),
        "2024-05-01T01:23:34Z"
    );
}

TEST(TinkoffMarketTest, TestGetCandles) {
    TradingBot::TinkoffMarket tMarket(10, true);
    Helpers::VectorView<TradingBot::Candle> candles = tMarket.getCandles();

    EXPECT_EQ(candles.size(), 0);

    // TradingBot::BacktestMarket market(candles);

    // TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(
    //     &market,
    //     {300, 0, 100, 100, 0, TradingBot::Balance().asAssetA()},
    //     {1, 1},
    //     {100, 200}
    // );
    // strategy.run();

    // EXPECT_EQ(market.getCandles().size(), 0);
    // EXPECT_EQ(market.getOrderHistory().size(), 0);
    // EXPECT_EQ(market.getBalanceHistory().size(), 0);
    // TradingBot::plot("TestAutoFitStrategyOnTinkoffMarketHistory.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());
    EXPECT_EQ(false, true);
}
