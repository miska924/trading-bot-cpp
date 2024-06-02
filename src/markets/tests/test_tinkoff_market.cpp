#include <gtest/gtest.h>

#include "markets/tinkoff_market.h"
#include "strategies/macd_strategy.h"
#include "strategies/autofit_strategy.h"
#include "plotting/plotting.h"
#include "helpers/date_time.h"


// TODO: MOCK TINKOFF MARKET and run tests

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

// TEST(TinkoffMarketTest, TestGetCandles) {
//     TradingBot::TinkoffMarket tMarket(1000, true);
//     Helpers::VectorView<TradingBot::Candle> candles = tMarket.getCandles();

//     EXPECT_EQ(tMarket.getCandles().size(), 1000);
//     for (int i = 0; i < 120; ++i) {
//         sleep(1);
//         tMarket.update();
//         EXPECT_EQ(tMarket.getCandles().size(), 1000);
//         std::cerr << "---------" << std::endl;
//         std::cerr << Helpers::DateTime(tMarket.getCandles().back().time) << std::endl;
//     }

//     // TradingBot::BacktestMarket market(candles);

//     // TradingBot::AutoFitStrategy<TradingBot::MACDHoldSlowStrategy> strategy(
//     //     &market,
//     //     {300, 0, 100, 100, 0, TradingBot::Balance().asAssetA()},
//     //     {1, 1},
//     //     {100, 200}
//     // );
//     // strategy.run();

//     // EXPECT_EQ(market.getCandles().size(), 0);
//     // EXPECT_EQ(market.getOrderHistory().size(), 0);
//     // EXPECT_EQ(market.getBalanceHistory().size(), 0);
//     // TradingBot::plot("TestAutoFitStrategyOnTinkoffMarketHistory.png", market.getCandles(), market.getOrderHistory(), market.getBalanceHistory());
//     EXPECT_EQ(false, true);
// }
