#include "helpers/http_connection.h"

namespace Helpers {

    HttpConnection::HttpConnection() {
        curl_global_init(CURL_GLOBAL_ALL);
        curl_ = curl_easy_init();
    }

    HttpConnection::~HttpConnection() {
        curl_easy_cleanup(curl_);
    }

    bool HttpConnection::Post(
        const std::string& url,
        const std::vector<std::string>& headers,
        const std::string& data,
        std::string& response
    ) {
        if (!curl_) {
            return false;
        }

        struct curl_slist* header_list = NULL;
        for (const std::string& header : headers) {
            header_list = curl_slist_append(header_list, header.c_str());
        }
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_list);

        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_POST, 1L);
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl_);
        return (res == CURLE_OK);
    }

    bool HttpConnection::Get(
        const std::string& url,
        const std::vector<std::string>& headers,
        std::string& response
    ) {
        if (!curl_) {
            return false;
        }

        struct curl_slist* header_list = NULL;
        for (const std::string& header : headers) {
            header_list = curl_slist_append(header_list, header.c_str());
        }
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_list);

        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_POST, 0L);
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl_);
        return (res == CURLE_OK);
    }

    size_t HttpConnection::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t realsize = size * nmemb;
        std::string* str = static_cast<std::string*>(userp);
        str->append(static_cast<char*>(contents), realsize);
        return realsize;
    }

} // namespace Helpers
