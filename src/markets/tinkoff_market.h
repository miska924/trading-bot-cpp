#pragma once

#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "markets/market.h"
#include "helpers/vector_view.h"
#include "helpers/http_connection.h"


namespace TradingBot {
    const std::string DATETIME_FORMAT_S = "%Y-%m-%dT%H:%M:%SZ";
    const std::string DATETIME_FORMAT_MS = "%Y-%m-%dT%H:%M:%S.%3NZ";
    const std::string TINKOFF_SB_API_URL = "https://sandbox-invest-public-api.tinkoff.ru/rest/tinkoff.public.invest.api.contract.v1.";
    const int HISTORY_BATCH_SIZE = 50;

    time_t ParseDateTime(const char* datetimeString, const std::string& datetimeFormat = DATETIME_FORMAT_S);
    std::string DateTime(time_t time,  const std::string& datetimeFormat = DATETIME_FORMAT_S);

    class TinkoffMarket : public Market {
    public:
        TinkoffMarket(size_t candlesCount = 2000, bool verbose = false);
        ~TinkoffMarket() override;

        bool order(Order order) override;
        bool update() override;
        Helpers::VectorView<Candle> getCandles() const override;
        Balance getBalance() override;
    private:
        bool verbose;
        size_t candlesCount;

        std::vector<Candle> candles;
        std::vector<std::string> headers;
        Helpers::HttpConnection connection;
        std::string SBToken;
        CURL* curl;

        std::string accountId;
        std::string assetA;
        std::string assetB;
        nlohmann::json instrument;

        nlohmann::json Post(const std::string& handler, const std::string& data = "{}");
        double GetCurrency(
            const nlohmann::json& positions,
            const std::string& assetKey,
            const std::string& currency
        );
        bool LoadCandlesHistory(size_t candlesCount = 1);
        void CloseAllAccounts();
    };

} // namespace TradingBot
