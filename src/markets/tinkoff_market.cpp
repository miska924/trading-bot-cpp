#include "markets/tinkoff_market.h"
#include "helpers/date_time.h"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <thread>
#include <math.h>
#include <fmt/core.h>


namespace TradingBot {

    double doubleFromTinkoffFormat(const nlohmann::json& dict) {
        int units = std::atoi(dict["units"].get<std::string>().c_str());
        int nanoDigits = std::to_string(abs(dict["nano"].get<int>())).size();
        int nano = dict["nano"].get<int>();
        return units + double(nano) / pow(10, nanoDigits);
    }

    nlohmann::json TinkoffMarket::Post(const std::string& handler, nlohmann::json data) {
        if (verbose) {
            std::cerr << handler << "..." << data.dump(2) << std::endl;
        }

        std::string response;
        if (!connection.Post(
            TINKOFF_SB_API_URL + handler,
            headers,
            data.dump(),
            response
        )) {
            throw std::runtime_error(handler + " request failed");
        }

        if (response.empty()) {
            throw std::runtime_error(handler + " empty response");
        }

        nlohmann::json result = nlohmann::json::parse(response);

        if (verbose || result.contains("code") && result["code"] != 0) {
            std::cerr << handler << ": " << response << std::endl;
        }

        return result;
    }

    TinkoffMarket::TinkoffMarket(
        size_t candlesCount, bool verbose
    ) : verbose(verbose), candlesCount(candlesCount) {
        headers = {
            std::string("Authorization: Bearer ") + std::getenv("TINKOFF_SB_TOKEN"),
            "Content-Type: application/json",
            "accept: application/json",
        };

        CloseAllAccounts();

        accountId = Post("SandboxService/OpenSandboxAccount")["accountId"];
        assetA = "rub";
        assetB = "GAZP";

        Post(
            "SandboxService/SandboxPayIn",
            {
                {"accountId", accountId},
                {"amount", {
                    {"units", "10000"},
                    {"currency", assetA}
                }}
            }
        );

        instrument = Post(
            "InstrumentsService/ShareBy",
            GAZP_SHARE_BY
        )["instrument"];

        candleTimeDelta = 60;
        LoadCandlesHistory(candlesCount);
    }

    void TinkoffMarket::CloseAllAccounts() {
        nlohmann::json accounts = Post("SandboxService/GetSandboxAccounts");

        for (const auto& account: accounts["accounts"]) {
            Post(
                "SandboxService/CloseSandboxAccount",
                {
                    {"accountId", account["id"]}
                }
            );
        }
    }

    bool TinkoffMarket::LoadCandlesHistory(size_t candlesCount) {
        const bool init = candles.empty();
        time_t to = std::time(nullptr);
        time_t from = (init ? to - candleTimeDelta * this->candlesCount : candles.back().time);
        int partCount = (to - from + HISTORY_BATCH_SIZE * candleTimeDelta - 1) / (HISTORY_BATCH_SIZE * candleTimeDelta);

        bool updated = false;
        int part = 0;
        bool wasNotCompletedCandle = false;
        while ((init && candles.size() < this->candlesCount) || (!init && part < partCount)) {
            nlohmann::json historyResponse = Post(
                "MarketDataService/GetCandles",
                {
                    {"instrumentId", instrument["uid"]},
                    {"interval", "CANDLE_INTERVAL_1_MIN"},
                    {"from", Helpers::DateTime(
                        (init ? to - (part + 1) * HISTORY_BATCH_SIZE * candleTimeDelta : from + part * HISTORY_BATCH_SIZE * candleTimeDelta)
                    )},
                    {"to", Helpers::DateTime(
                        int((init ? to - part * HISTORY_BATCH_SIZE * candleTimeDelta : from + (part + 1) * HISTORY_BATCH_SIZE * candleTimeDelta))
                    )}
                }
            )["candles"];

            if (historyResponse.empty()) {
                ++part;
                continue;
            }

            for (int i = (init ? historyResponse.size() - 1 : 0); (init ? i >= 0 : i < historyResponse.size()); (init ? --i : ++i)) {
                if (init && candles.size() >= candlesCount) {
                    break;
                }
                const auto& candle = historyResponse[i];
                time_t candleTime = Helpers::ParseDateTime(candle["time"].get<std::string>().c_str());
                if (!candles.empty() && (
                    (init && candles.front().time <= candleTime) ||
                    (!init && candles.back().time >= candleTime))) {
                    continue;
                }
                if (!candle.contains("isComplete") || !candle["isComplete"].get<bool>()) {
                    if (!init) {
                        wasNotCompletedCandle = true;
                    }
                    continue;
                }
                updated = true;
                Candle pushCandle = {
                    .time = candleTime,
                    .open = doubleFromTinkoffFormat(candle["open"]),
                    .high = doubleFromTinkoffFormat(candle["high"]),
                    .low = doubleFromTinkoffFormat(candle["low"]),
                    .close = doubleFromTinkoffFormat(candle["close"]),
                    .volume = std::stod(candle["volume"].get<std::string>())
                };
                candles.push_back(pushCandle);
            }
            ++part;
        }
        if (init) {
            reverse(candles.begin(), candles.end());
        }
        if (this->candlesCount * 2 < candles.size()) {
            int toRemove = candles.size() - this->candlesCount;
            candles = std::vector(
                candles.begin() + toRemove,
                candles.end()
            );
        }
        return updated;
    }

    TinkoffMarket::~TinkoffMarket() {
        std::string response;
        
        Post(
            "SandboxService/CloseSandboxAccount",
            {
                {"accountId", accountId}
            }
        );
    }

    bool TinkoffMarket::order(Order order) {
        if (order.side == OrderSide::RESET) {
            if (balance.assetB == 0) {
                return true;
            }
            Post(
                "OrdersService/PostOrder",
                {
                    {"quantity", std::to_string(abs(balance.assetB))},
                    {"direction", balance.assetB > 0 ? "ORDER_DIRECTION_SELL" : "ORDER_DIRECTION_BUY"},
                    {"accountId", accountId},
                    {"orderType", "ORDER_TYPE_MARKET"},
                    {"instrumentId", instrument["uid"]}
                }
            );
        } else if (order.side == OrderSide::BUY) {
            Post(
                "OrdersService/PostOrder",
                {
                    {"quantity", "1"},
                    {"direction", "ORDER_DIRECTION_BUY"},
                    {"accountId", accountId},
                    {"orderType", "ORDER_TYPE_MARKET"},
                    {"instrumentId", instrument["uid"]}
                }
            );
        } else if (order.side == OrderSide::SELL) {
            Post(
                "OrdersService/PostOrder",
                {
                    {"quantity", "1"},
                    {"direction", "ORDER_DIRECTION_SELL"},
                    {"accountId", accountId},
                    {"orderType", "ORDER_TYPE_MARKET"},
                    {"instrumentId", instrument["uid"]}
                }
            );
        }
        balance = getBalance();
        lastOrder = order;
        return true;
    }

    bool TinkoffMarket::update() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!Post(
            "MarketDataService/GetTradingStatus",
            {
                {"instrumentId", instrument["uid"]}
            }
        )["marketOrderAvailableFlag"]) {
            return LoadCandlesHistory(10);
        }
        return LoadCandlesHistory(10);
    }

    Helpers::VectorView<Candle> TinkoffMarket::getCandles() const {
        return candles;
    }

    double TinkoffMarket::GetCurrency(const nlohmann::json& positions, const std::string& assetKey, const std::string& asset) {
        for (const nlohmann::json& position : positions) {
            if (position.contains(assetKey) && position[assetKey] == asset) {
                return doubleFromTinkoffFormat(position);
            }
        }
        return 0;
    }

    Balance TinkoffMarket::getBalance() {
        nlohmann::json positions = Post(
            "OperationsService/GetPositions",
            {
                {"accountId", accountId}
            }
        );
        return balance = {
            .assetA = GetCurrency(positions["money"], "currency", assetA),
            .assetB = GetCurrency(positions["securities"], "balance", assetB)
        };
    }

} // namespace TradingBot
