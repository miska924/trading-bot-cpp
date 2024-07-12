#pragma once

#include <map>
#include <string>

#include <cpr/cpr.h>
#include <jsoncpp/json/json.h>


namespace TradingBot {

    const int RETRY_COUNT = 3;
    const int RETRY_DELAY_MS = 1000;

    class HttpClient {
    public:
        HttpClient(const std::string& endpoint);
        ~HttpClient();

        void setHeader(const std::string& name, const std::string& value);
        void removeHeader(const std::string& name);
        void clearHeaders();

        Json::Value get(const std::string& url);
        Json::Value post(
            const std::string& url,
            const Json::Value& body = Json::Value(Json::ValueType::objectValue)
        );

    private:
        std::map<std::string, std::string> headers_;
        std::string endpoint;
    };

} // namespace TradingBot
