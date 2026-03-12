#pragma once
#include <string>
#include <curl/curl.h>
 
namespace FlightDS {
 
class APIClient {
private:
    std::string baseUrl;
    long timeoutSeconds;
    std::string lastError;
 
    static size_t writeCallback(void* contents,
                                size_t size,
                                size_t nmemb,
                                std::string* output);
 
public:
    explicit APIClient(const std::string& baseUrl,
                       long timeoutSeconds = 10);
    ~APIClient() = default;
 
    std::string get(const std::string& endpoint);
    std::string getWithParams(const std::string& endpoint,
                              const std::string& params);
 
    bool isAvailable();
    std::string getLastError() const;
    void setBaseUrl(const std::string& url);
};
 
} // namespace FlightDS
