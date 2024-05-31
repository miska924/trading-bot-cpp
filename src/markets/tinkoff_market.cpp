#include "markets/tinkoff_market.h"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <thread>
#include <math.h>
#include <fmt/core.h>


namespace TradingBot {

    time_t ParseDateTime(const char* datetimeString, const std::string& datetimeFormat)
    {
        struct tm tmStruct;
        strptime(datetimeString, datetimeFormat.c_str(), &tmStruct);
        return mktime(&tmStruct);
    }
    
    std::string DateTime(time_t time,  const std::string& datetimeFormat)
    {
        // std::cerr << "DateTime {" << std::endl;
        char buffer[90];
        struct tm* timeinfo = gmtime(&time);
        strftime(buffer, sizeof(buffer), datetimeFormat.c_str(), timeinfo);
        // std::cerr << "}" << std::endl;
        return buffer;
    }

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

        candleTimeDelta = 15 * 60;
        LoadCandlesHistory(candlesCount);
    }

    void TinkoffMarket::CloseAllAccounts() {
        nlohmann::json accounts = Post("SandboxService/GetSandboxAccounts");

        for (const auto& account: accounts["accounts"]) {
            std::cerr << "account: " << account["id"].get<std::string>() << std::endl;
            Post(
                "SandboxService/CloseSandboxAccount",
                {
                    {"accountId", account["id"]}
                }
            );
        }
    }

    bool TinkoffMarket::LoadCandlesHistory(size_t candlesCount) {
        time_t to = std::time(nullptr);
        time_t from = to - candleTimeDelta * candlesCount;
        int partCount = (to - from + HISTORY_BATCH_SIZE * candleTimeDelta - 1) / (HISTORY_BATCH_SIZE * candleTimeDelta);

        bool updated = false;
        for (size_t part = 0; part < partCount; ++part) {
            if (verbose) {
                std::cerr << DateTime(
                    from + part * HISTORY_BATCH_SIZE * candleTimeDelta
                ) << std::endl;
            }
            nlohmann::json historyResponse = Post(
                "MarketDataService/GetCandles",
                {
                    {"instrumentId", instrument["uid"]},
                    {"interval", "CANDLE_INTERVAL_15_MIN"},
                    {"from", DateTime(
                        from + part * HISTORY_BATCH_SIZE * candleTimeDelta
                    )},
                    {"to", DateTime(
                        std::min(int(from + (part + 1) * HISTORY_BATCH_SIZE * candleTimeDelta), int(to))
                    )}
                }
            )["candles"];

            for (const auto& candle: historyResponse) {
                time_t candleTime = ParseDateTime(candle["time"].get<std::string>().c_str());
                if (verbose) {
                    std::cerr << " --- " << DateTime(candleTime) << std::endl;
                }
                if (!candles.empty() && candles.back().time >= candleTime) {
                    if (verbose) {
                        std::cerr << "skipping candle" << std::endl;
                    }
                    continue;
                }
                if (!candle.contains("isComplete") || !candle["isComplete"].get<bool>()) {
                    if (verbose) {
                        std::cerr << "not complete candle" << std::endl;
                    }
                    continue;
                }
                updated = true;
                candles.push_back(Candle{
                    .time = candleTime,
                    .open = doubleFromTinkoffFormat(candle["open"]),
                    .high = doubleFromTinkoffFormat(candle["high"]),
                    .low = doubleFromTinkoffFormat(candle["low"]),
                    .close = doubleFromTinkoffFormat(candle["close"]),
                    .volume = std::stod(candle["volume"].get<std::string>())
                });
            }
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
            return false;
        }
        return LoadCandlesHistory(10);
    }

    Helpers::VectorView<Candle> TinkoffMarket::getCandles() const {
        return std::vector(candles.begin(), candles.end());
    }

    double TinkoffMarket::GetCurrency(const nlohmann::json& positions, const std::string& assetKey, const std::string& asset) {
        for (const nlohmann::json& position : positions) {
            if (position[assetKey] == asset) {
                return doubleFromTinkoffFormat(position);
            }
        }
        return 0;
    }

    Balance TinkoffMarket::getBalance() {
        nlohmann::json positions = Post(
            "OperationsService/GetPositions",
            {
                {"brokerAccountId", accountId}
            }
        );

        return {
            .assetA = GetCurrency(positions["money"], "currency", assetA),
            .assetB = GetCurrency(positions["securities"], "balance", assetB)
        };
    }

} // namespace TradingBot
