#pragma once
#include <string>
#include <iostream>
 
namespace FlightDS {
 
class Flight {
private:
    std::string icao24;
    std::string callsign;
    std::string originCountry;
    double longitude;
    double latitude;
    double altitude;
    double velocity;
    double heading;
    bool onGround;
    long lastContact;
 
public:
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
 
    void setAltitude(double alt);
    void setVelocity(double vel);
 
    double getVelocityKmh() const;
    double getAltitudeFeet() const;
    std::string getStatusString() const;
    void display() const;
    std::string toJson() const;
};
 
} // namespace FlightDS
