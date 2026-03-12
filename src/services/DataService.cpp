#include "services/DataService.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <ctime>
#include <stdexcept>
 
using namespace std;
 
namespace FlightDS {
 
DataService::DataService()
    : apiClient(make_unique<APIClient>("https://opensky-network.org")),
      lastFetchTime(0), cacheDurationSeconds(30) {
    initializeAirports();
}
 
DataService::DataService(int cacheDuration)
    : apiClient(make_unique<APIClient>("https://opensky-network.org")),
      lastFetchTime(0), cacheDurationSeconds(cacheDuration) {
    initializeAirports();
}
 
void DataService::initializeAirports() {
    airports.emplace_back("VIDP", "Indira Gandhi International Airport",
                          "India", 28.5562, 77.1000, 300.0);
    airports.emplace_back("VABB", "Chhatrapati Shivaji Maharaj International",
                          "India", 19.0896, 72.8656, 300.0);
    airports.emplace_back("VOBL", "Kempegowda International Airport",
                          "India", 13.1986, 77.7066, 300.0);
    airports.emplace_back("VOMM", "Chennai International Airport",
                          "India", 12.9900, 80.1693, 300.0);
    airports.emplace_back("KSEA", "Seattle-Tacoma International",
                          "United States", 47.4502, -122.3088, 300.0);
}
 
Flight DataService::parseFlightFromState(const json& state) {
    auto safeDouble = [](const json& j) -> double {
        return j.is_null() ? 0.0 : j.get<double>();
    };
    auto safeLong = [](const json& j) -> long {
        return j.is_null() ? 0L : j.get<long>();
    };
    auto safeString = [](const json& j) -> string {
        if (j.is_null()) return "";
        string s = j.get<string>();
        while (!s.empty() && (s.back() == ' ' || s.back() == '\0'))
            s.pop_back();
        return s;
    };
 
    return Flight(
        safeString(state[0]),
        safeString(state[1]),
        safeString(state[2]),
        safeDouble(state[5]),
        safeDouble(state[6]),
        safeDouble(state[7]),
        safeDouble(state[9]),
        safeDouble(state[10]),
        state[8].is_null() ? false : state[8].get<bool>(),
        safeLong(state[4])
    );
}
 
vector<Flight> DataService::parseFlights(const string& jsonData) {
    vector<Flight> flights;
    try {
        json root = json::parse(jsonData);
        if (!root.contains("states") || root["states"].is_null()) {
            cout << "[DataService] No flight states in response.\n";
            return flights;
        }
        for (const auto& state : root["states"]) {
            if (state.size() >= 11)
                flights.push_back(parseFlightFromState(state));
        }
    } catch (const json::exception& e) {
        cerr << "[DataService] JSON parse error: " << e.what() << "\n";
    }
    return flights;
}
 
vector<Flight> DataService::fetchLiveFlights() {
    long now = static_cast<long>(time(nullptr));
    if (!cachedFlights.empty() && (now - lastFetchTime) < cacheDurationSeconds) {
        cout << "[DataService] Returning cached data ("
             << cachedFlights.size() << " flights).\n";
        return cachedFlights;
    }
    cout << "[DataService] Fetching live flight data from OpenSky...\n";
    try {
        string response = apiClient->get("/api/states/all");
        cachedFlights = parseFlights(response);
        lastFetchTime = now;
        cout << "[DataService] Fetched " << cachedFlights.size() << " flights.\n";
    } catch (const exception& e) {
        cerr << "[DataService] Fetch error: " << e.what() << "\n";
        if (!cachedFlights.empty()) return cachedFlights;
    }
    return cachedFlights;
}
 
vector<Flight> DataService::fetchFlightsByRegion(double minLat, double maxLat,
                                                  double minLon, double maxLon) {
    string params = "lamin=" + to_string(minLat) +
                    "&lamax=" + to_string(maxLat) +
                    "&lomin=" + to_string(minLon) +
                    "&lomax=" + to_string(maxLon);
    try {
        string response = apiClient->getWithParams("/api/states/all", params);
        return parseFlights(response);
    } catch (const exception& e) {
        cerr << "[DataService] Region fetch error: " << e.what() << "\n";
        return {};
    }
}
 
vector<Flight> DataService::getAirborneFlights() {
    auto all = fetchLiveFlights();
    vector<Flight> result;
    copy_if(all.begin(), all.end(), back_inserter(result),
            [](const Flight& f) { return !f.isOnGround(); });
    return result;
}
 
vector<Flight> DataService::getFlightsByCountry(const string& country) {
    auto all = fetchLiveFlights();
    vector<Flight> result;
    copy_if(all.begin(), all.end(), back_inserter(result),
            [&country](const Flight& f) {
                return f.getOriginCountry() == country;
            });
    return result;
}
 
vector<Flight> DataService::getFlightsNearAirport(const string& airportCode) {
    auto all = fetchLiveFlights();
    vector<Flight> result;
    Airport* target = nullptr;
    for (auto& a : airports) {
        if (a.getCode() == airportCode) { target = &a; break; }
    }
    if (!target) {
        cerr << "[DataService] Airport not found: " << airportCode << "\n";
        return result;
    }
    copy_if(all.begin(), all.end(), back_inserter(result),
            [&target](const Flight& f) { return target->isFlightNearby(f); });
    return result;
}
 
vector<Flight> DataService::getHighAltitudeFlights(double minAltitudeMetres) {
    auto all = fetchLiveFlights();
    vector<Flight> result;
    copy_if(all.begin(), all.end(), back_inserter(result),
            [minAltitudeMetres](const Flight& f) {
                return f.getAltitude() >= minAltitudeMetres;
            });
    return result;
}
 
int DataService::getTotalFlightCount() {
    return static_cast<int>(fetchLiveFlights().size());
}
 
int DataService::getAirborneCount() {
    return static_cast<int>(getAirborneFlights().size());
}
 
double DataService::getAverageAltitude() {
    auto airborne = getAirborneFlights();
    if (airborne.empty()) return 0.0;
    double total = accumulate(airborne.begin(), airborne.end(), 0.0,
        [](double sum, const Flight& f) { return sum + f.getAltitude(); });
    return total / airborne.size();
}
 
double DataService::getAverageVelocity() {
    auto airborne = getAirborneFlights();
    if (airborne.empty()) return 0.0;
    double total = accumulate(airborne.begin(), airborne.end(), 0.0,
        [](double sum, const Flight& f) { return sum + f.getVelocityKmh(); });
    return total / airborne.size();
}
 
string DataService::flightsToJson(const vector<Flight>& flights) {
    ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < flights.size(); ++i) {
        oss << flights[i].toJson();
        if (i + 1 < flights.size()) oss << ",";
    }
    oss << "]";
    return oss.str();
}
 
string DataService::statsToJson() {
    ostringstream oss;
    oss << fixed;
    oss << "{"
        << "\"total_flights\":" << getTotalFlightCount() << ","
        << "\"airborne_flights\":" << getAirborneCount() << ","
        << "\"average_altitude_m\":" << setprecision(1) << getAverageAltitude() << ","
        << "\"average_velocity_kmh\":" << getAverageVelocity()
        << "}";
    return oss.str();
}
 
void DataService::addAirport(const Airport& airport) {
    airports.push_back(airport);
}
 
vector<Airport> DataService::getAirports() const { return airports; }
 
} // namespace FlightDS
