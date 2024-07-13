#include "markets/clients/tinkoff/client.h"

#include "helpers/date_time.h"
#include <iostream>


namespace TradingBot {

    TinkoffClient::TinkoffClient(
        const std::string& token
    ): httpClient(TINKOFF_ENDPOINT) {
        httpClient.setHeader("Content-Type", "application/json");
        httpClient.setHeader("Authorization", "Bearer " + token);
        httpClient.setHeader("Accept", "application/json");
    }

    Json::Value TinkoffClient::InstrumentsService_ShareBy(
        const std::string& id
    ) {
        Json::Value body;
        body["idType"] = "INSTRUMENT_ID_TYPE_TICKER";
        body["id"] = id;
        body["classCode"] = "TQBR";

        return httpClient.post(
            TINKOFF_URL_PREFIX + "InstrumentsService/ShareBy",
            body
        );
    }

    Json::Value TinkoffClient::UsersService_GetAccounts() {
        return httpClient.post(
            TINKOFF_URL_PREFIX + "UsersService/GetAccounts"
        );
    }

    Json::Value TinkoffClient::MarketDataService_GetCandles(
        const std::string& instrumentId,
        const CandleTimeDelta interval,
        time_t from,
        time_t to
    ) {
        Json::Value body;
        body["instrumentId"] = instrumentId;
        body["interval"] = toString(interval);
        body["classCode"] = "TQBR";
        body["from"] = Helpers::DateTime(from);
        body["to"] = Helpers::DateTime(to);

        std::cerr << body.toStyledString() << std::endl;

        Json::Value response = httpClient.post(
            TINKOFF_URL_PREFIX + "MarketDataService/GetCandles",
            body
        );
        while (!response["candles"]) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            response = httpClient.post(
                TINKOFF_URL_PREFIX + "MarketDataService/GetCandles",
                body
            );
        };

        return response;
    }

    Json::Value TinkoffClient::OrdersService_PostOrder(
        uint quantity,
        OrderDirection direction,
        const std::string& accoutId,
        const std::string& instrumentId
    ) {
        Json::Value body;
        body["quantity"] = quantity;
        body["direction"] = toString(direction);
        body["accountId"] = accoutId;
        body["orderType"] = "ORDER_TYPE_MARKET";
        body["instrumentId"] = instrumentId;

        return httpClient.post(
            TINKOFF_URL_PREFIX + "OrdersService/PostOrder",
            body
        );
    }

    Json::Value TinkoffClient::OperationsService_GetPositions(const std::string& accountId) {
        Json::Value body;
        body["accountId"] = accountId;
        return httpClient.post(
            TINKOFF_URL_PREFIX + "OperationsService/GetPositions",
            body
        );
    }

} // namespace TradingBot
