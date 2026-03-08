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

/**
 * @brief Core business logic service.
 * Fetches, parses, filters and processes flight data.
 * Demonstrates: Service-Oriented Architecture (SOA).
 */
class DataService {
private:
    std::unique_ptr<APIClient> apiClient;
    std::vector<Flight> cachedFlights;
    std::vector<Airport> airports;
    long lastFetchTime;
    int cacheDurationSeconds;

    // Parse raw JSON from OpenSky API into Flight objects
    std::vector<Flight> parseFlights(const std::string& jsonData);

    // Parse individual state vector from OpenSky response
    Flight parseFlightFromState(const json& state);

public:
    DataService();
    explicit DataService(int cacheDuration);
    ~DataService() = default;

    // Initialize predefined airports for monitoring
    void initializeAirports();

    // Fetch live flights from OpenSky Network API (FREE, no auth)
    std::vector<Flight> fetchLiveFlights();

    // Fetch flights over a specific bounding box
    std::vector<Flight> fetchFlightsByRegion(double minLat,
                                              double maxLat,
                                              double minLon,
                                              double maxLon);

    // Data processing methods
    std::vector<Flight> getAirborneFlights();
    std::vector<Flight> getFlightsByCountry(const std::string& country);
    std::vector<Flight> getFlightsNearAirport(const std::string& airportCode);
    std::vector<Flight> getHighAltitudeFlights(double minAltitudeMetres);

    // Statistics
    int getTotalFlightCount();
    int getAirborneCount();
    double getAverageAltitude();
    double getAverageVelocity();

    // Serialization
    std::string flightsToJson(const std::vector<Flight>& flights);
    std::string statsToJson();

    // Airport management
    void addAirport(const Airport& airport);
    std::vector<Airport> getAirports() const;
};

} // namespace FlightDS