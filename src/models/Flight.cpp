#include "models/Flight.h"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace FlightDS {

// Default constructor
Flight::Flight()
    : icao24(""), callsign("UNKNOWN"), originCountry(""),
      longitude(0.0), latitude(0.0), altitude(0.0),
      velocity(0.0), heading(0.0), onGround(false), lastContact(0) {}

// Parameterized constructor
Flight::Flight(const std::string& icao24,
               const std::string& callsign,
               const std::string& originCountry,
               double longitude, double latitude,
               double altitude, double velocity,
               double heading, bool onGround, long lastContact)
    : icao24(icao24), callsign(callsign), originCountry(originCountry),
      longitude(longitude), latitude(latitude), altitude(altitude),
      velocity(velocity), heading(heading),
      onGround(onGround), lastContact(lastContact) {}

// Getters
std::string Flight::getIcao24() const      { return icao24; }
std::string Flight::getCallsign() const    { return callsign; }
std::string Flight::getOriginCountry() const { return originCountry; }
double Flight::getLongitude() const        { return longitude; }
double Flight::getLatitude() const         { return latitude; }
double Flight::getAltitude() const         { return altitude; }
double Flight::getVelocity() const         { return velocity; }
double Flight::getHeading() const          { return heading; }
bool Flight::isOnGround() const            { return onGround; }
long Flight::getLastContact() const        { return lastContact; }

// Setters
void Flight::setAltitude(double alt) { altitude = alt; }
void Flight::setVelocity(double vel) { velocity = vel; }

// Utility — convert m/s to km/h
double Flight::getVelocityKmh() const {
    return velocity * 3.6;
}

// Utility — convert metres to feet
double Flight::getAltitudeFeet() const {
    return altitude * 3.28084;
}

// Utility — status string
std::string Flight::getStatusString() const {
    return onGround ? "On Ground" : "Airborne";
}

// Display flight info to console
void Flight::display() const {
    std::cout << "-------------------------------\n";
    std::cout << "Callsign    : " << callsign << "\n";
    std::cout << "ICAO24      : " << icao24 << "\n";
    std::cout << "Country     : " << originCountry << "\n";
    std::cout << "Status      : " << getStatusString() << "\n";
    std::cout << "Position    : " << std::fixed << std::setprecision(4)
              << latitude << ", " << longitude << "\n";
    std::cout << "Altitude    : " << std::fixed << std::setprecision(0)
              << altitude << " m (" << getAltitudeFeet() << " ft)\n";
    std::cout << "Velocity    : " << std::fixed << std::setprecision(1)
              << getVelocityKmh() << " km/h\n";
    std::cout << "Heading     : " << heading << " deg\n";
}

// Serialize to JSON string
std::string Flight::toJson() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "{"
        << "\"icao24\":\"" << icao24 << "\","
        << "\"callsign\":\"" << callsign << "\","
        << "\"origin_country\":\"" << originCountry << "\","
        << "\"latitude\":" << latitude << ","
        << "\"longitude\":" << longitude << ","
        << "\"altitude_m\":" << std::setprecision(1) << altitude << ","
        << "\"altitude_ft\":" << getAltitudeFeet() << ","
        << "\"velocity_ms\":" << velocity << ","
        << "\"velocity_kmh\":" << getVelocityKmh() << ","
        << "\"heading\":" << heading << ","
        << "\"on_ground\":" << (onGround ? "true" : "false") << ","
        << "\"status\":\"" << getStatusString() << "\","
        << "\"last_contact\":" << lastContact
        << "}";
    return oss.str();
}

} // namespace FlightDS