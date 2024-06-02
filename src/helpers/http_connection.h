#include <curl/curl.h>
#include <string>
#include <vector>


namespace Helpers {

    class HttpConnection {
    public:
        HttpConnection();
        ~HttpConnection();
        bool Post(
            const std::string& url,
            const std::vector<std::string>& headers,
            const std::string& data,
            std::string& response
        );
        bool Get(
            const std::string& url,
            const std::vector<std::string>& headers,
            std::string& response
        );
    private:
        CURL* curl_ = nullptr;
        static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    };

} // namespace Helpers
