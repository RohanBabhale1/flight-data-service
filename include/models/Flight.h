#pragma once
#include <string>
#include <iostream>

namespace FlightDS {

/**
 * @brief Represents a single aircraft in flight.
 * Models real-world flight data from OpenSky Network API.
 */
class Flight {
private:
    std::string icao24;        // Unique aircraft identifier
    std::string callsign;      // Flight callsign (e.g., "AI101")
    std::string originCountry; // Country of origin
    double longitude;          // Current longitude
    double latitude;           // Current latitude
    double altitude;           // Altitude in metres (baro)
    double velocity;           // Speed in m/s
    double heading;            // Direction in degrees
    bool onGround;             // Is aircraft on ground?
    long lastContact;          // Unix timestamp of last contact

public:
    // Constructors
    Flight();
    Flight(const std::string& icao24,
           const std::string& callsign,
           const std::string& originCountry,
           double longitude,
           double latitude,
           double altitude,
           double velocity,
           double heading,
           bool onGround,
           long lastContact);

    // Getters
    std::string getIcao24() const;
    std::string getCallsign() const;
    std::string getOriginCountry() const;
    double getLongitude() const;
    double getLatitude() const;
    double getAltitude() const;
    double getVelocity() const;
    double getHeading() const;
    bool isOnGround() const;
    long getLastContact() const;

    // Setters
    void setAltitude(double alt);
    void setVelocity(double vel);

    // Utility methods
    double getVelocityKmh() const;       // Convert m/s to km/h
    double getAltitudeFeet() const;      // Convert metres to feet
    std::string getStatusString() const; // "Airborne" or "On Ground"
    void display() const;                // Print flight info
    std::string toJson() const;          // Serialize to JSON string
};

} // namespace FlightDS