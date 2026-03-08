#include "services/APIClient.h"
#include <iostream>
#include <stdexcept>

namespace FlightDS {

// libcurl write callback — appends received data to string
size_t APIClient::writeCallback(void* contents,
                                 size_t size,
                                 size_t nmemb,
                                 std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

APIClient::APIClient(const std::string& baseUrl, long timeoutSeconds)
    : baseUrl(baseUrl), timeoutSeconds(timeoutSeconds), lastError("") {
    // Initialize libcurl globally (call once per program)
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

// Core GET request
std::string APIClient::get(const std::string& endpoint) {
    CURL* curl = curl_easy_init();
    std::string response;
    std::string url = baseUrl + endpoint;

    if (!curl) {
        lastError = "Failed to initialize CURL handle";
        throw std::runtime_error(lastError);
    }

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "FlightDataService/1.0");

    // SSL verification (set to 0 for dev, 1 for production)
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        lastError = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("HTTP GET failed: " + lastError);
    }

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    if (httpCode != 200) {
        lastError = "HTTP error code: " + std::to_string(httpCode);
        curl_easy_cleanup(curl);
        throw std::runtime_error(lastError);
    }

    curl_easy_cleanup(curl);
    return response;
}

// GET with query parameters
std::string APIClient::getWithParams(const std::string& endpoint,
                                      const std::string& params) {
    return get(endpoint + "?" + params);
}

// Check if the API is reachable
bool APIClient::isAvailable() {
    try {
        CURL* curl = curl_easy_init();
        if (!curl) return false;

        curl_easy_setopt(curl, CURLOPT_URL, baseUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        return res == CURLE_OK;
    } catch (...) {
        return false;
    }
}

std::string APIClient::getLastError() const { return lastError; }
void APIClient::setBaseUrl(const std::string& url) { baseUrl = url; }

} // namespace FlightDS