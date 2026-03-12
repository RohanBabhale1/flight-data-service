#pragma once
#include <string>
#include <memory>
#include "services/DataService.h"
#include "third_party/httplib.h"

namespace FlightDS {

/**
 * @brief REST API server — produces HTTP endpoints.
 * Exposes flight data through a clean REST interface.
 * Demonstrates: producing RESTful APIs (Boeing JD requirement).
 *
 * Endpoints:
 *   GET /health              — Server health check
 *   GET /api/flights         — All live flights
 *   GET /api/flights/airborne — Only airborne flights
 *   GET /api/flights/stats   — Aggregated statistics
 *   GET /api/airports        — Monitored airports
 *   GET /api/flights/country?name=India — Filter by country
 */
class RestServer {
private:
    httplib::Server server;
    std::shared_ptr<DataService> dataService;
    int port;
    std::string host;

    // Register all route handlers
    void setupRoutes();

    // Route handlers
    void handleHealth(const httplib::Request& req,
                      httplib::Response& res);
    void handleAllFlights(const httplib::Request& req,
                          httplib::Response& res);
    void handleAirborneFlights(const httplib::Request& req,
                                httplib::Response& res);
    void handleStats(const httplib::Request& req,
                     httplib::Response& res);
    void handleAirports(const httplib::Request& req,
                        httplib::Response& res);
    void handleFlightsByCountry(const httplib::Request& req,
                                 httplib::Response& res);

    // Utility
    void setCORSHeaders(httplib::Response& res);
    std::string wrapResponse(const std::string& data,
                              const std::string& status = "success");

public:
    RestServer(int port = 8080, const std::string& host = "0.0.0.0");
    explicit RestServer(std::shared_ptr<DataService> ds,
                        int port = 8080);
    ~RestServer() = default;

    void start();
    void stop();
    int getPort() const;
};

} // namespace FlightDS