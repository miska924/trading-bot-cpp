#pragma once

#include <map>
#include <string>

#include <drogon/HttpClient.h>


namespace TradingBot {

    class HttpClient {
    public:
        HttpClient(const std::string& endpoint);
        ~HttpClient();

        void setHeader(const std::string& name, const std::string& value);
        void removeHeader(const std::string& name);
        void clearHeaders();

        Json::Value get(const std::string& url);
        Json::Value post(const std::string& url, const Json::Value& body);

    private:
        Json::Value sendRequest(drogon::HttpRequestPtr req);

        drogon::HttpClientPtr client_;
        std::map<std::string, std::string> headers_;
        std::thread loopThread_;
    };

} // namespace TradingBot
