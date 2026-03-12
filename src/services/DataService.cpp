#include "services/DataService.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <ctime>
#include <stdexcept>

namespace FlightDS {

DataService::DataService()
    : apiClient(std::make_unique<APIClient>("https://opensky-network.org")),
      lastFetchTime(0), cacheDurationSeconds(30) {
    initializeAirports();
}

DataService::DataService(int cacheDuration)
    : apiClient(std::make_unique<APIClient>("https://opensky-network.org")),
      lastFetchTime(0), cacheDurationSeconds(cacheDuration) {
    initializeAirports();
}

// Setup airports to monitor (India-focused for Boeing India role)
void DataService::initializeAirports() {
    airports.emplace_back("VIDP", "Indira Gandhi International Airport",
                          "India", 28.5562, 77.1000, 300.0);
    airports.emplace_back("VABB", "Chhatrapati Shivaji Maharaj International",
                          "India", 19.0896, 72.8656, 300.0);
    airports.emplace_back("VOBL", "Kempegowda International Airport",
                          "India", 13.1986, 77.7066, 300.0); // Bangalore
    airports.emplace_back("VOMM", "Chennai International Airport",
                          "India", 12.9900, 80.1693, 300.0);
    airports.emplace_back("KSEA", "Seattle-Tacoma International",
                          "United States", 47.4502, -122.3088, 300.0); // Boeing HQ
}

// Parse a single flight state from OpenSky's state vector array
Flight DataService::parseFlightFromState(const json& state) {
    // OpenSky state vector format (array indices):
    // [0]=icao24, [1]=callsign, [2]=origin_country,
    // [3]=time_position, [4]=last_contact,
    // [5]=longitude, [6]=latitude, [7]=baro_altitude,
    // [8]=on_ground, [9]=velocity, [10]=true_track

    auto safeDouble = [](const json& j) -> double {
        return j.is_null() ? 0.0 : j.get<double>();
    };
    auto safeLong = [](const json& j) -> long {
        return j.is_null() ? 0L : j.get<long>();
    };
    auto safeString = [](const json& j) -> std::string {
        if (j.is_null()) return "";
        std::string s = j.get<std::string>();
        // Trim trailing whitespace from callsign
        while (!s.empty() && (s.back() == ' ' || s.back() == '\0'))
            s.pop_back();
        return s;
    };

    return Flight(
        safeString(state[0]),   // icao24
        safeString(state[1]),   // callsign
        safeString(state[2]),   // origin country
        safeDouble(state[5]),   // longitude
        safeDouble(state[6]),   // latitude
        safeDouble(state[7]),   // altitude (metres)
        safeDouble(state[9]),   // velocity (m/s)
        safeDouble(state[10]),  // heading
        state[8].is_null() ? false : state[8].get<bool>(), // on_ground
        safeLong(state[4])      // last_contact
    );
}

// Parse all flights from OpenSky JSON response
std::vector<Flight> DataService::parseFlights(const std::string& jsonData) {
    std::vector<Flight> flights;
    try {
        json root = json::parse(jsonData);
        if (!root.contains("states") || root["states"].is_null()) {
            std::cout << "[DataService] No flight states in response.\n";
            return flights;
        }
        for (const auto& state : root["states"]) {
            if (state.size() >= 11) {
                flights.push_back(parseFlightFromState(state));
            }
        }
    } catch (const json::exception& e) {
        std::cerr << "[DataService] JSON parse error: " << e.what() << "\n";
    }
    return flights;
}

// Fetch all live flights (global, no auth needed for OpenSky)
std::vector<Flight> DataService::fetchLiveFlights() {
    long now = static_cast<long>(time(nullptr));

    // Use cached data if still fresh
    if (!cachedFlights.empty() &&
        (now - lastFetchTime) < cacheDurationSeconds) {
        std::cout << "[DataService] Returning cached data ("
                  << cachedFlights.size() << " flights).\n";
        return cachedFlights;
    }

    std::cout << "[DataService] Fetching live flight data from OpenSky...\n";
    try {
        std::string response = apiClient->get("/api/states/all");
        cachedFlights = parseFlights(response);
        lastFetchTime = now;
        std::cout << "[DataService] Fetched " << cachedFlights.size()
                  << " flights.\n";
    } catch (const std::exception& e) {
        std::cerr << "[DataService] Fetch error: " << e.what() << "\n";
        // Return cached if available, even if stale
        if (!cachedFlights.empty()) return cachedFlights;
    }
    return cachedFlights;
}

// Fetch flights over India (bounding box)
std::vector<Flight> DataService::fetchFlightsByRegion(
    double minLat, double maxLat, double minLon, double maxLon) {

    std::string params = "lamin=" + std::to_string(minLat) +
                         "&lamax=" + std::to_string(maxLat) +
                         "&lomin=" + std::to_string(minLon) +
                         "&lomax=" + std::to_string(maxLon);
    try {
        std::string response = apiClient->getWithParams("/api/states/all",
                                                         params);
        return parseFlights(response);
    } catch (const std::exception& e) {
        std::cerr << "[DataService] Region fetch error: " << e.what() << "\n";
        return {};
    }
}

// Filter: only airborne flights
std::vector<Flight> DataService::getAirborneFlights() {
    auto all = fetchLiveFlights();
    std::vector<Flight> result;
    std::copy_if(all.begin(), all.end(), std::back_inserter(result),
                 [](const Flight& f) { return !f.isOnGround(); });
    return result;
}

// Filter: flights from specific country
std::vector<Flight> DataService::getFlightsByCountry(
    const std::string& country) {
    auto all = fetchLiveFlights();
    std::vector<Flight> result;
    std::copy_if(all.begin(), all.end(), std::back_inserter(result),
                 [&country](const Flight& f) {
                     return f.getOriginCountry() == country;
                 });
    return result;
}

// Filter: flights near a specific airport
std::vector<Flight> DataService::getFlightsNearAirport(
    const std::string& airportCode) {
    auto all = fetchLiveFlights();
    std::vector<Flight> result;

    // Find airport
    Airport* target = nullptr;
    for (auto& a : airports) {
        if (a.getCode() == airportCode) { target = &a; break; }
    }
    if (!target) {
        std::cerr << "[DataService] Airport not found: " << airportCode << "\n";
        return result;
    }

    std::copy_if(all.begin(), all.end(), std::back_inserter(result),
                 [&target](const Flight& f) {
                     return target->isFlightNearby(f);
                 });
    return result;
}

// Filter: high altitude flights
std::vector<Flight> DataService::getHighAltitudeFlights(
    double minAltitudeMetres) {
    auto all = fetchLiveFlights();
    std::vector<Flight> result;
    std::copy_if(all.begin(), all.end(), std::back_inserter(result),
                 [minAltitudeMetres](const Flight& f) {
                     return f.getAltitude() >= minAltitudeMetres;
                 });
    return result;
}

// Stats
int DataService::getTotalFlightCount() {
    return static_cast<int>(fetchLiveFlights().size());
}

int DataService::getAirborneCount() {
    return static_cast<int>(getAirborneFlights().size());
}

double DataService::getAverageAltitude() {
    auto airborne = getAirborneFlights();
    if (airborne.empty()) return 0.0;
    double total = std::accumulate(airborne.begin(), airborne.end(), 0.0,
        [](double sum, const Flight& f) { return sum + f.getAltitude(); });
    return total / airborne.size();
}

double DataService::getAverageVelocity() {
    auto airborne = getAirborneFlights();
    if (airborne.empty()) return 0.0;
    double total = std::accumulate(airborne.begin(), airborne.end(), 0.0,
        [](double sum, const Flight& f) { return sum + f.getVelocityKmh(); });
    return total / airborne.size();
}

// Serialize a vector of flights to JSON array
std::string DataService::flightsToJson(const std::vector<Flight>& flights) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < flights.size(); ++i) {
        oss << flights[i].toJson();
        if (i + 1 < flights.size()) oss << ",";
    }
    oss << "]";
    return oss.str();
}

// Serialize stats summary to JSON
std::string DataService::statsToJson() {
    std::ostringstream oss;
    oss << std::fixed;
    oss << "{"
        << "\"total_flights\":" << getTotalFlightCount() << ","
        << "\"airborne_flights\":" << getAirborneCount() << ","
        << "\"average_altitude_m\":" << std::setprecision(1)
        << getAverageAltitude() << ","
        << "\"average_velocity_kmh\":" << getAverageVelocity()
        << "}";
    return oss.str();
}

void DataService::addAirport(const Airport& airport) {
    airports.push_back(airport);
}

std::vector<Airport> DataService::getAirports() const { return airports; }

} // namespace FlightDS