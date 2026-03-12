#include "server/RestServer.h"
#include <iostream>
#include <sstream>
 
using namespace std;
 
namespace FlightDS {
 
RestServer::RestServer(int port, const string& host)
    : dataService(make_shared<DataService>()),
      port(port), host(host) {
    setupRoutes();
}
 
RestServer::RestServer(shared_ptr<DataService> ds, int port)
    : dataService(ds), port(port), host("0.0.0.0") {
    setupRoutes();
}
 
void RestServer::setCORSHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_header("Content-Type", "application/json");
}
 
string RestServer::wrapResponse(const string& data, const string& status) {
    ostringstream oss;
    oss << "{\"status\":\"" << status << "\",\"data\":" << data << "}";
    return oss.str();
}
 
void RestServer::setupRoutes() {
    server.Get("/health", [this](const httplib::Request& req, httplib::Response& res) {
        handleHealth(req, res);
    });
 
    server.Get("/api/flights", [this](const httplib::Request& req, httplib::Response& res) {
        handleAllFlights(req, res);
    });
 
    server.Get("/api/flights/airborne", [this](const httplib::Request& req, httplib::Response& res) {
        handleAirborneFlights(req, res);
    });
 
    server.Get("/api/flights/stats", [this](const httplib::Request& req, httplib::Response& res) {
        handleStats(req, res);
    });
 
    server.Get("/api/airports", [this](const httplib::Request& req, httplib::Response& res) {
        handleAirports(req, res);
    });
 
    server.Get("/api/flights/country", [this](const httplib::Request& req, httplib::Response& res) {
        handleFlightsByCountry(req, res);
    });
}
 
void RestServer::handleHealth(const httplib::Request&, httplib::Response& res) {
    setCORSHeaders(res);
    res.set_content("{\"status\":\"ok\","
                    "\"service\":\"FlightDataService\","
                    "\"version\":\"1.0.0\"}",
                    "application/json");
}
 
void RestServer::handleAllFlights(const httplib::Request&, httplib::Response& res) {
    setCORSHeaders(res);
    try {
        auto flights = dataService->fetchLiveFlights();
        string json = dataService->flightsToJson(flights);
        res.set_content(wrapResponse(json), "application/json");
    } catch (const exception& e) {
        res.status = 500;
        res.set_content(string("{\"status\":\"error\",\"message\":\"") + e.what() + "\"}",
                        "application/json");
    }
}
 
void RestServer::handleAirborneFlights(const httplib::Request&, httplib::Response& res) {
    setCORSHeaders(res);
    try {
        auto flights = dataService->getAirborneFlights();
        string json = dataService->flightsToJson(flights);
        res.set_content(wrapResponse(json), "application/json");
    } catch (const exception& e) {
        res.status = 500;
        res.set_content(string("{\"status\":\"error\",\"message\":\"") + e.what() + "\"}",
                        "application/json");
    }
}
 
void RestServer::handleStats(const httplib::Request&, httplib::Response& res) {
    setCORSHeaders(res);
    try {
        string stats = dataService->statsToJson();
        res.set_content(wrapResponse(stats), "application/json");
    } catch (const exception& e) {
        res.status = 500;
        res.set_content(string("{\"status\":\"error\",\"message\":\"") + e.what() + "\"}",
                        "application/json");
    }
}
 
void RestServer::handleAirports(const httplib::Request&, httplib::Response& res) {
    setCORSHeaders(res);
    auto airports = dataService->getAirports();
    ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < airports.size(); ++i) {
        oss << airports[i].toJson();
        if (i + 1 < airports.size()) oss << ",";
    }
    oss << "]";
    res.set_content(wrapResponse(oss.str()), "application/json");
}
 
void RestServer::handleFlightsByCountry(const httplib::Request& req, httplib::Response& res) {
    setCORSHeaders(res);
    string country = req.has_param("name") ? req.get_param_value("name") : "India";
    try {
        auto flights = dataService->getFlightsByCountry(country);
        string json = dataService->flightsToJson(flights);
        res.set_content(wrapResponse(json), "application/json");
    } catch (const exception& e) {
        res.status = 500;
        res.set_content(string("{\"status\":\"error\",\"message\":\"") + e.what() + "\"}",
                        "application/json");
    }
}
 
void RestServer::start() {
    cout << "\n========================================\n";
    cout << "  Flight Data Service v1.0.0\n";
    cout << "  Listening on http://" << host << ":" << port << "\n";
    cout << "========================================\n";
    cout << "  GET /health\n";
    cout << "  GET /api/flights\n";
    cout << "  GET /api/flights/airborne\n";
    cout << "  GET /api/flights/stats\n";
    cout << "  GET /api/airports\n";
    cout << "  GET /api/flights/country?name=India\n";
    cout << "========================================\n\n";
    server.listen(host.c_str(), port);
}
 
void RestServer::stop() { server.stop(); }
int RestServer::getPort() const { return port; }
 
} // namespace FlightDS
