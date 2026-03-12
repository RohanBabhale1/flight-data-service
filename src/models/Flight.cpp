#include "models/Flight.h"
#include <cmath>
#include <sstream>
#include <iomanip>
 
using namespace std;
 
namespace FlightDS {
 
Flight::Flight()
    : icao24(""), callsign("UNKNOWN"), originCountry(""),
      longitude(0.0), latitude(0.0), altitude(0.0),
      velocity(0.0), heading(0.0), onGround(false), lastContact(0) {}
 
Flight::Flight(const string& icao24, const string& callsign,
               const string& originCountry,
               double longitude, double latitude,
               double altitude, double velocity,
               double heading, bool onGround, long lastContact)
    : icao24(icao24), callsign(callsign), originCountry(originCountry),
      longitude(longitude), latitude(latitude), altitude(altitude),
      velocity(velocity), heading(heading),
      onGround(onGround), lastContact(lastContact) {}
 
string Flight::getIcao24() const      { return icao24; }
string Flight::getCallsign() const    { return callsign; }
string Flight::getOriginCountry() const { return originCountry; }
double Flight::getLongitude() const   { return longitude; }
double Flight::getLatitude() const    { return latitude; }
double Flight::getAltitude() const    { return altitude; }
double Flight::getVelocity() const    { return velocity; }
double Flight::getHeading() const     { return heading; }
bool   Flight::isOnGround() const     { return onGround; }
long   Flight::getLastContact() const { return lastContact; }
 
void Flight::setAltitude(double alt) { altitude = alt; }
void Flight::setVelocity(double vel) { velocity = vel; }
 
double Flight::getVelocityKmh() const { return velocity * 3.6; }
double Flight::getAltitudeFeet() const { return altitude * 3.28084; }
 
string Flight::getStatusString() const {
    return onGround ? "On Ground" : "Airborne";
}
 
void Flight::display() const {
    cout << "-------------------------------\n";
    cout << "Callsign    : " << callsign << "\n";
    cout << "ICAO24      : " << icao24 << "\n";
    cout << "Country     : " << originCountry << "\n";
    cout << "Status      : " << getStatusString() << "\n";
    cout << "Position    : " << fixed << setprecision(4)
         << latitude << ", " << longitude << "\n";
    cout << "Altitude    : " << fixed << setprecision(0)
         << altitude << " m (" << getAltitudeFeet() << " ft)\n";
    cout << "Velocity    : " << fixed << setprecision(1)
         << getVelocityKmh() << " km/h\n";
    cout << "Heading     : " << heading << " deg\n";
}
 
string Flight::toJson() const {
    ostringstream oss;
    oss << fixed << setprecision(4);
    oss << "{"
        << "\"icao24\":\"" << icao24 << "\","
        << "\"callsign\":\"" << callsign << "\","
        << "\"origin_country\":\"" << originCountry << "\","
        << "\"latitude\":" << latitude << ","
        << "\"longitude\":" << longitude << ","
        << "\"altitude_m\":" << setprecision(1) << altitude << ","
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
