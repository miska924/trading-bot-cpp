#include "http/client.h"

#include <drogon/drogon.h>
#include <drogon/HttpResponse.h>
#include <future>
#include <thread>

#ifdef __linux__
#include <sys/socket.h>
#include <netinet/tcp.h>
#endif

namespace TradingBot {

    HttpClient::HttpClient(const std::string& endpoint) {
        client_ = drogon::HttpClient::newHttpClient(endpoint);
        // Запуск event loop в отдельном потоке
        loopThread_ = std::thread([]() {
            drogon::app().run();
        });
    }

    HttpClient::~HttpClient() {
        // Остановка event loop и завершение потока
        drogon::app().quit();
        if (loopThread_.joinable()) {
            loopThread_.join();
        }
    }

    void HttpClient::setHeader(const std::string& name, const std::string& value) {
        headers_[name] = value;
    }

    void HttpClient::removeHeader(const std::string& name) {
        headers_.erase(name);
    }

    void HttpClient::clearHeaders() {
        headers_.clear();
    }

Json::Value HttpClient::sendRequest(drogon::HttpRequestPtr req) {
    auto promise = std::make_shared<std::promise<Json::Value>>();
    std::future<Json::Value> future = promise->get_future();

    client_->sendRequest(req, [promise](drogon::ReqResult result, const drogon::HttpResponsePtr &response) mutable {
        if (result != drogon::ReqResult::Ok) {
            std::cerr << "Error while sending request to server! Result: " << result << std::endl;
            promise->set_value(Json::Value());
            return;
        }
        if (response->statusCode() != 200) {
            std::cerr << "Error while sending request to server! Status code: " << response->statusCode() << std::endl;
            promise->set_value(Json::Value());
            return;
        }
        if (response->contentType() == drogon::CT_APPLICATION_JSON) {
            auto jsonObject = response->getJsonObject();
            if (jsonObject) {
                promise->set_value(*jsonObject);
            } else {
                promise->set_value(Json::Value());
            }
        } else {
            std::cerr << "Response is not in JSON format!" << std::endl;
            promise->set_value(Json::Value());
        }
    });

    return future.get();
}

    Json::Value HttpClient::get(const std::string& url) {
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Get);
        req->setPath(url);

        for (const auto& header : headers_) {
            req->addHeader(header.first, header.second);
        }

        return sendRequest(req);
    }

    Json::Value HttpClient::post(const std::string& url, const Json::Value& body) {
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Post);
        req->setPath(url);
        req->setBody(body.toStyledString());
        req->setContentTypeCode(drogon::CT_APPLICATION_JSON);

        for (const auto& header : headers_) {
            req->addHeader(header.first, header.second);
        }

        return sendRequest(req);
    }

} // namespace TradingBot
