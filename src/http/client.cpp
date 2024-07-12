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
        // [TODO] need singleton with drogon::app thread here
        client_ = drogon::HttpClient::newHttpClient(endpoint);

        loopThread_ = std::thread([]() {
            drogon::app().run();
        });
    }

    HttpClient::~HttpClient() {
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
    std::cerr << "REQUEST:" << std::endl;
    std::cerr << req->getMethod() << std::endl;
    std::cerr << req->getPath() << std::endl;
    for (const auto& header : req->getHeaders()) {
        std::cerr << header.first << ": " << header.second << std::endl;
    }
    std::cerr << req->getBody() << std::endl;
    std::cerr << req->getContentType() << std::endl;
    std::cerr << std::endl;

    auto promise = std::make_shared<std::promise<Json::Value>>();
    std::future<Json::Value> future = promise->get_future();

    client_->sendRequest(req, [promise](drogon::ReqResult result, const drogon::HttpResponsePtr &response) mutable {
        if (result != drogon::ReqResult::Ok) {
            std::cerr << "Error while sending request to server! Result: " << result << std::endl;
            promise->set_value(Json::Value());
            throw std::runtime_error("Error while sending request to server! Result: " + std::to_string(int(result)));
        }
        if (response->statusCode() != 200) {
            std::cerr << "Error while sending request to server! Status code: " << response->statusCode() << std::endl;
            throw std::runtime_error("Error while sending request to server! Status code: "
                + std::to_string(response->statusCode()) + " " + std::string(response->getBody()));
        }
        std::cerr << "========================" << std::endl;
        std::cerr << "'" << response->getBody() << "'" << std::endl;
        if (response->contentType() == drogon::CT_APPLICATION_JSON) {
            auto jsonObject = response->getJsonObject();
            if (jsonObject) {
                promise->set_value(*jsonObject);
            } else {
                promise->set_value(Json::Value());
            }
        } else {
            std::cerr << "Response is not in JSON format!" << std::endl;
            throw std::runtime_error("Response is not in JSON format!");
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
        std::cerr << std::endl << url << std::endl << body.toStyledString() << std::endl;

        auto req = drogon::HttpRequest::newHttpJsonRequest(body);
        req->setPassThrough(true);
        // req->clearHeaders();
        req->setMethod(drogon::Post);
        req->setPath(url);
        // req->setBody(body.toStyledString());
        // req->setPathEncode(false);
        // req->setCustomContentTypeString("application/json")
        // req->setBody(body.toStyledString().c_str());
        // req->setContentTypeString("application/json");
        

        for (const auto& header : headers_) {
            req->addHeader(header.first, header.second);
        }

        // std::cerr << ->toStyledString() << std::endl;

        return sendRequest(req);
    }

} // namespace TradingBot
