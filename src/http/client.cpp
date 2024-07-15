#include "http/client.h"


namespace TradingBot {

    Json::Value toJson(const std::string& s) {
        std::stringstream stream;
        stream << s;
        Json::Value json;
        stream >> json;
        return json;
    }

    HttpClient::HttpClient(const std::string& endpoint) :
        endpoint(endpoint)
    {}

    HttpClient::~HttpClient() {}

    void HttpClient::setHeader(const std::string& name, const std::string& value) {
        headers_[name] = value;
    }

    void HttpClient::removeHeader(const std::string& name) {
        headers_.erase(name);
    }

    void HttpClient::clearHeaders() {
        headers_.clear();
    }

    Json::Value HttpClient::get(const std::string& url) {
        cpr::Session session;

        for (const auto& header : headers_) {
            session.UpdateHeader({{header.first, header.second}});
        }

        session.SetUrl(endpoint + url);
        cpr::Response response = session.Get();

        for (int i = 0; i < RETRY_COUNT; ++i) {
            if (response.status_code == 200) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
            response = session.Get();
        }

        if (response.status_code != 200) {
            throw std::runtime_error("Failed to get " + url + ": " + response.text);
        }
        
        return toJson(response.text);
    }

    Json::Value HttpClient::post(const std::string& url, const Json::Value& body) {
        cpr::Session session;
        session.SetBody(body.toStyledString());
        session.SetUrl(endpoint + url);

        for (const auto& header : headers_) {
            session.UpdateHeader({{header.first, header.second}});
        }

        cpr::Response response = session.Post();

        while (response.status_code == 429) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            response = session.Post();
        }

        if (response.status_code != 200) {
            throw std::runtime_error(
                "Failed to post " + url + ": "
                + std::to_string(int(response.status_code))
                + ", " + response.reason
                + ", " + response.text
                + ", " + response.status_line);
        }

        return toJson(response.text);
    }

} // namespace TradingBot
