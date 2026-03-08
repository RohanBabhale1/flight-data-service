#include "server/RestServer.h"
#include <iostream>
#include <sstream>

namespace FlightDS {

RestServer::RestServer(int port, const std::string& host)
    : dataService(std::make_shared<DataService>()),
      port(port), host(host) {
    setupRoutes();
}

RestServer::RestServer(std::shared_ptr<DataService> ds, int port)
    : dataService(ds), port(port), host("0.0.0.0") {
    setupRoutes();
}

// Add CORS headers to allow browser access
void RestServer::setCORSHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_header("Content-Type", "application/json");
}

// Wrap response data in standard envelope
std::string RestServer::wrapResponse(const std::string& data,
                                      const std::string& status) {
    std::ostringstream oss;
    oss << "{\"status\":\"" << status << "\",\"data\":" << data << "}";
    return oss.str();
}

// Register all routes
void RestServer::setupRoutes() {

    // GET /health — server health check
    server.Get("/health", [this](const httplib::Request& req,
                                  httplib::Response& res) {
        handleHealth(req, res);
    });

    // GET /api/flights — all live flights
    server.Get("/api/flights", [this](const httplib::Request& req,
                                       httplib::Response& res) {
        handleAllFlights(req, res);
    });

    // GET /api/flights/airborne — only airborne
    server.Get("/api/flights/airborne",
               [this](const httplib::Request& req, httplib::Response& res) {
                   handleAirborneFlights(req, res);
               });

    // GET /api/flights/stats — statistics
    server.Get("/api/flights/stats",
               [this](const httplib::Request& req, httplib::Response& res) {
                   handleStats(req, res);
               });

    // GET /api/airports — monitored airports list
    server.Get("/api/airports", [this](const httplib::Request& req,
                                        httplib::Response& res) {
        handleAirports(req, res);
    });

    // GET /api/flights/country?name=India
    server.Get("/api/flights/country",
               [this](const httplib::Request& req, httplib::Response& res) {
                   handleFlightsByCountry(req, res);
               });
}

void RestServer::handleHealth(const httplib::Request&,
                               httplib::Response& res) {
    setCORSHeaders(res);
    res.set_content("{\"status\":\"ok\","
                    "\"service\":\"FlightDataService\","
                    "\"version\":\"1.0.0\"}",
                    "application/json");
}

void RestServer::handleAllFlights(const httplib::Request&,
                                   httplib::Response& res) {
    setCORSHeaders(res);
    try {
        auto flights = dataService->fetchLiveFlights();
        std::string json = dataService->flightsToJson(flights);
        res.set_content(wrapResponse(json), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content("{\"status\":\"error\",\"message\":\""
                        + std::string(e.what()) + "\"}",
                        "application/json");
    }
}

void RestServer::handleAirborneFlights(const httplib::Request&,
                                        httplib::Response& res) {
    setCORSHeaders(res);
    try {
        auto flights = dataService->getAirborneFlights();
        std::string json = dataService->flightsToJson(flights);
        res.set_content(wrapResponse(json), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content("{\"status\":\"error\",\"message\":\""
                        + std::string(e.what()) + "\"}",
                        "application/json");
    }
}

void RestServer::handleStats(const httplib::Request&,
                              httplib::Response& res) {
    setCORSHeaders(res);
    try {
        std::string stats = dataService->statsToJson();
        res.set_content(wrapResponse(stats), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content("{\"status\":\"error\",\"message\":\""
                        + std::string(e.what()) + "\"}",
                        "application/json");
    }
}

void RestServer::handleAirports(const httplib::Request&,
                                 httplib::Response& res) {
    setCORSHeaders(res);
    auto airports = dataService->getAirports();
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < airports.size(); ++i) {
        oss << airports[i].toJson();
        if (i + 1 < airports.size()) oss << ",";
    }
    oss << "]";
    res.set_content(wrapResponse(oss.str()), "application/json");
}

void RestServer::handleFlightsByCountry(const httplib::Request& req,
                                         httplib::Response& res) {
    setCORSHeaders(res);
    std::string country = "India"; // default
    if (req.has_param("name")) {
        country = req.get_param_value("name");
    }
    try {
        auto flights = dataService->getFlightsByCountry(country);
        std::string json = dataService->flightsToJson(flights);
        res.set_content(wrapResponse(json), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content("{\"status\":\"error\",\"message\":\""
                        + std::string(e.what()) + "\"}",
                        "application/json");
    }
}

void RestServer::start() {
    std::cout << "\n========================================\n";
    std::cout << "  Flight Data Service v1.0.0\n";
    std::cout << "  Listening on http://" << host << ":" << port << "\n";
    std::cout << "========================================\n";
    std::cout << "  Endpoints:\n";
    std::cout << "  GET /health\n";
    std::cout << "  GET /api/flights\n";
    std::cout << "  GET /api/flights/airborne\n";
    std::cout << "  GET /api/flights/stats\n";
    std::cout << "  GET /api/airports\n";
    std::cout << "  GET /api/flights/country?name=India\n";
    std::cout << "========================================\n\n";
    server.listen(host.c_str(), port);
}

void RestServer::stop() { server.stop(); }
int RestServer::getPort() const { return port; }

} // namespace FlightDS