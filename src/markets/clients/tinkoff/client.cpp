#include "markets/clients/tinkoff/client.h"

#include "helpers/date_time.h"


namespace TradingBot {

    TinkoffClient::TinkoffClient(
        const std::string& token
    ): httpClient(TINKOFF_ENDPOINT) {
        std::cerr << "TOKEN: " << token << std::endl;
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
            TINKOFF_URL_PREFIX + "Instruments/ShareBy",
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

        return httpClient.post(
            TINKOFF_URL_PREFIX + "MarketDataService/GetCandles",
            body
        );
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
