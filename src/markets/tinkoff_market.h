#pragma once

#include <string>
#include <deque>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "markets/market.h"
#include "helpers/vector_view.h"
#include "helpers/http_connection.h"


namespace TradingBot {

    enum TinkoffMarketMode {
        SB = 0,
        PROD = 1,
    };

    enum CandleTimeDelta {
        CANDLE_1_MIN = 60,
        CANDLE_2_MIN = 2 * CANDLE_1_MIN,
        CANDLE_3_MIN = 3 * CANDLE_1_MIN,
        CANDLE_5_MIN = 5 * CANDLE_1_MIN,
        CANDLE_10_MIN = 10 * CANDLE_1_MIN,
        CANDLE_15_MIN = 15 * CANDLE_1_MIN,
        CANDLE_30_MIN = 30 * CANDLE_1_MIN,
        CANDLE_1_HOUR = 60 * CANDLE_1_MIN,
        CANDLE_2_HOUR = 2 * CANDLE_1_HOUR,
        CANDLE_4_HOUR = 4 * CANDLE_1_HOUR,
        CANDLE_1_DAY = 24 * CANDLE_1_MIN,
        CANDLE_1_WEEK = 7 * CANDLE_1_DAY,
        CANDLE_1_MONTH = 30 * CANDLE_1_DAY,
    };

    std::string toTinkoffString(CandleTimeDelta delta);

    const std::string TINKOFF_SB_API_URL = "https://sandbox-invest-public-api.tinkoff.ru/rest/tinkoff.public.invest.api.contract.v1.";
    const std::string TINKOFF_API_URL = "https://invest-public-api.tinkoff.ru/rest/tinkoff.public.invest.api.contract.v1.";
    const int HISTORY_BATCH_SIZE = 60;

    class TinkoffMarket : public Market {
    public:
        TinkoffMarket(
            TinkoffMarketMode mode = TinkoffMarketMode::SB,
            size_t candlesCount = 2000,
            CandleTimeDelta timeDelta = CANDLE_1_HOUR,
            std::string asset = "GAZP",
            int verbose = 1
        );
        ~TinkoffMarket() override;

        bool order(Order order) override;
        bool update() override;
        Helpers::VectorView<Candle> getCandles() const override;
        Balance getBalance() override;
    private:
        TinkoffMarketMode mode;
        CandleTimeDelta timeDelta;

        int verbose;
        size_t candlesCount;
        std::string candleTimeDeltaString;

        std::vector<Candle> candles;
        std::vector<std::string> headers;
        Helpers::HttpConnection connection;
        std::string SBToken;
        CURL* curl;

        std::string accountId;
        std::string assetA;
        std::string assetB;
        nlohmann::json instrument;

        nlohmann::json Post(const std::string& handler, nlohmann::json data = nlohmann::json::object());
        double GetCurrency(
            const nlohmann::json& positions,
            const std::string& assetKey,
            const std::string& currency
        );
        bool LoadCandlesHistory();
        void CloseAllAccounts();
    };

} // namespace TradingBot
