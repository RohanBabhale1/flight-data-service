#pragma once
#include <vector>
#include <string>
#include <memory>
#include "models/Flight.h"
#include "models/Airport.h"
#include "services/APIClient.h"
#include "third_party/json.hpp"
 
using json = nlohmann::json;
 
namespace FlightDS {
 
class DataService {
private:
    std::unique_ptr<APIClient> apiClient;
    std::vector<Flight> cachedFlights;
    std::vector<Airport> airports;
    long lastFetchTime;
    int cacheDurationSeconds;
 
    std::vector<Flight> parseFlights(const std::string& jsonData);
    Flight parseFlightFromState(const json& state);
 
public:
    DataService();
    explicit DataService(int cacheDuration);
    ~DataService() = default;
 
    void initializeAirports();
 
    std::vector<Flight> fetchLiveFlights();
    std::vector<Flight> fetchFlightsByRegion(double minLat,
                                             double maxLat,
                                             double minLon,
                                             double maxLon);
 
    std::vector<Flight> getAirborneFlights();
    std::vector<Flight> getFlightsByCountry(const std::string& country);
    std::vector<Flight> getFlightsNearAirport(const std::string& airportCode);
    std::vector<Flight> getHighAltitudeFlights(double minAltitudeMetres);
 
    int getTotalFlightCount();
    int getAirborneCount();
    double getAverageAltitude();
    double getAverageVelocity();
 
    std::string flightsToJson(const std::vector<Flight>& flights);
    std::string statsToJson();
 
    void addAirport(const Airport& airport);
    std::vector<Airport> getAirports() const;
};
 
} // namespace FlightDS
