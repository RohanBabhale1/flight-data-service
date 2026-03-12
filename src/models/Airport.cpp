#include "models/Airport.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>
 
using namespace std;
 
namespace FlightDS {
 
const double PI = 3.14159265358979323846;
const double EARTH_RADIUS_KM = 6371.0;
 
Airport::Airport()
    : code(""), name(""), country(""),
      latitude(0.0), longitude(0.0), radiusKm(200.0) {}
 
Airport::Airport(const string& code, const string& name,
                 const string& country,
                 double latitude, double longitude, double radiusKm)
    : code(code), name(name), country(country),
      latitude(latitude), longitude(longitude), radiusKm(radiusKm) {}
 
string Airport::getCode() const    { return code; }
string Airport::getName() const    { return name; }
string Airport::getCountry() const { return country; }
double Airport::getLatitude() const  { return latitude; }
double Airport::getLongitude() const { return longitude; }
double Airport::getRadiusKm() const  { return radiusKm; }
 
double Airport::distanceTo(double lat, double lon) const {
    double dLat = (lat - latitude) * PI / 180.0;
    double dLon = (lon - longitude) * PI / 180.0;
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(latitude * PI / 180.0) * cos(lat * PI / 180.0) *
               sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS_KM * c;
}
 
bool Airport::isFlightNearby(const Flight& flight) const {
    if (flight.isOnGround()) return false;
    return distanceTo(flight.getLatitude(), flight.getLongitude()) <= radiusKm;
}
 
void Airport::display() const {
    cout << "Airport: " << name << " (" << code << ")\n";
    cout << "Country: " << country << "\n";
    cout << "Coords : " << latitude << ", " << longitude << "\n";
    cout << "Radius : " << radiusKm << " km\n";
}
 
string Airport::toJson() const {
    ostringstream oss;
    oss << fixed << setprecision(4);
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
