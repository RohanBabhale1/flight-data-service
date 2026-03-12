#pragma once
#include <string>
#include <vector>
#include "Flight.h"

namespace FlightDS {

/**
 * @brief Represents an airport with its geographic region.
 * Used to filter and group flights by region.
 */
class Airport {
private:
    std::string code;      // ICAO code (e.g., "VIDP" for Delhi)
    std::string name;      // Full airport name
    std::string country;   // Country name
    double latitude;       // Airport latitude
    double longitude;      // Airport longitude
    double radiusKm;       // Monitoring radius in km

public:
    Airport();
    Airport(const std::string& code,
            const std::string& name,
            const std::string& country,
            double latitude,
            double longitude,
            double radiusKm = 200.0);

    // Getters
    std::string getCode() const;
    std::string getName() const;
    std::string getCountry() const;
    double getLatitude() const;
    double getLongitude() const;
    double getRadiusKm() const;

    // Check if a flight is within monitoring radius
    bool isFlightNearby(const Flight& flight) const;

    // Calculate distance to a coordinate (Haversine formula)
    double distanceTo(double lat, double lon) const;

    void display() const;
    std::string toJson() const;
};

} // namespace FlightDS