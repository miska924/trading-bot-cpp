#pragma once

#include <string>
#include <deque>

#include <json/json.h>

#include "markets/market.h"
#include "helpers/vector_view.h"
#include "http/client.h"
#include "markets/clients/tinkoff/client.h"
#include "markets/clients/tinkoff/utils.h"


namespace TradingBot {

    const int HISTORY_BATCH_SIZE = 60 * 24;

    class TinkoffMarket : public Market {
    public:
        TinkoffMarket(
            size_t candlesCount = 2000,
            CandleTimeDelta timeDelta = CandleTimeDelta::CANDLE_1_HOUR,
            std::string asset = "GAZP",
            int verbose = 1
        );
        virtual ~TinkoffMarket() override = default;

        bool order(Order order) override;
        bool update() override;
        Helpers::VectorView<Candle> getCandles() const override;
        Balance getBalance() const override;
        double getFee() const override;
    private:
        CandleTimeDelta timeDelta;

        int verbose;
        size_t candlesCount;
        std::string candleTimeDeltaString;

        std::vector<Candle> candles;
        std::vector<std::string> headers;
        TinkoffClient tinkoffClient;

        std::string accountId;
        std::string assetA;
        std::string assetB;
        Json::Value instrument;

        double getCurrency(
            const Json::Value& positions,
            const std::string& assetKey,
            const std::string& currency
        );
        bool LoadCandlesHistory();
        void CloseAllAccounts();

        void updateBalance();
        void performOrder(double amount, OrderDirection direction);
    };

} // namespace TradingBot
