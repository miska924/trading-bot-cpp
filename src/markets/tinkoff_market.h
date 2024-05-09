#pragma once

#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "markets/market.h"
#include "helpers/vector_view.h"
#include "helpers/http_connection.h"


namespace TradingBot {

    const std::string TINKOFF_SB_API_URL = "https://sandbox-invest-public-api.tinkoff.ru/rest/tinkoff.public.invest.api.contract.v1.";

    class TinkoffMarket : public Market {
    public:
        TinkoffMarket();
        ~TinkoffMarket() override;

        bool order(Order order) override;
        bool update() override;
        Helpers::VectorView<Candle> getCandles() const override;
        Balance getBalance() override;
    private:
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
    };

} // namespace TradingBot
