#include "models/Airport.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace FlightDS {

const double PI = 3.14159265358979323846;
const double EARTH_RADIUS_KM = 6371.0;

Airport::Airport()
    : code(""), name(""), country(""),
      latitude(0.0), longitude(0.0), radiusKm(200.0) {}

Airport::Airport(const std::string& code,
                 const std::string& name,
                 const std::string& country,
                 double latitude, double longitude,
                 double radiusKm)
    : code(code), name(name), country(country),
      latitude(latitude), longitude(longitude), radiusKm(radiusKm) {}

// Getters
std::string Airport::getCode() const    { return code; }
std::string Airport::getName() const    { return name; }
std::string Airport::getCountry() const { return country; }
double Airport::getLatitude() const     { return latitude; }
double Airport::getLongitude() const    { return longitude; }
double Airport::getRadiusKm() const     { return radiusKm; }

// Haversine formula — calculates great-circle distance between two points
double Airport::distanceTo(double lat, double lon) const {
    double dLat = (lat - latitude) * PI / 180.0;
    double dLon = (lon - longitude) * PI / 180.0;

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(latitude * PI / 180.0) * cos(lat * PI / 180.0) *
               sin(dLon / 2) * sin(dLon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS_KM * c;
}

// Check if a flight is within this airport's monitoring radius
bool Airport::isFlightNearby(const Flight& flight) const {
    if (flight.isOnGround()) return false;
    double dist = distanceTo(flight.getLatitude(), flight.getLongitude());
    return dist <= radiusKm;
}

void Airport::display() const {
    std::cout << "Airport: " << name << " (" << code << ")\n";
    std::cout << "Country: " << country << "\n";
    std::cout << "Coords : " << latitude << ", " << longitude << "\n";
    std::cout << "Radius : " << radiusKm << " km\n";
}

std::string Airport::toJson() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "{"
        << "\"code\":\"" << code << "\","
        << "\"name\":\"" << name << "\","
        << "\"country\":\"" << country << "\","
        << "\"latitude\":" << latitude << ","
        << "\"longitude\":" << longitude << ","
        << "\"monitoring_radius_km\":" << radiusKm
        << "}";
    return oss.str();
}

} // namespace FlightDS