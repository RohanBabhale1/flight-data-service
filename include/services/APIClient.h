#pragma once
#include <string>
#include <curl/curl.h>

namespace FlightDS {

/**
 * @brief HTTP client service for consuming external REST APIs.
 * Wraps libcurl to provide clean GET/POST interface.
 * Demonstrates: consuming RESTful APIs (Boeing JD requirement).
 */
class APIClient {
private:
    std::string baseUrl;
    long timeoutSeconds;
    std::string lastError;

    // libcurl write callback
    static size_t writeCallback(void* contents,
                                 size_t size,
                                 size_t nmemb,
                                 std::string* output);

public:
    explicit APIClient(const std::string& baseUrl,
                       long timeoutSeconds = 10);
    ~APIClient() = default;

    // Core HTTP methods
    std::string get(const std::string& endpoint);
    std::string getWithParams(const std::string& endpoint,
                               const std::string& params);

    // Utility
    bool isAvailable();
    std::string getLastError() const;
    void setBaseUrl(const std::string& url);
};

} // namespace FlightDS