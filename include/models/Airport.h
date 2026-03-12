#pragma once
#include <string>
#include <vector>
#include "Flight.h"
 
namespace FlightDS {
 
class Airport {
private:
    std::string code;
    std::string name;
    std::string country;
    double latitude;
    double longitude;
    double radiusKm;
 
public:
    Airport();
    Airport(const std::string& code,
            const std::string& name,
            const std::string& country,
            double latitude,
            double longitude,
            double radiusKm = 200.0);
 
    std::string getCode() const;
    std::string getName() const;
    std::string getCountry() const;
    double getLatitude() const;
    double getLongitude() const;
    double getRadiusKm() const;
 
    bool isFlightNearby(const Flight& flight) const;
    double distanceTo(double lat, double lon) const;
    void display() const;
    std::string toJson() const;
};
 
} // namespace FlightDS
