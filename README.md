# Flight Data Management Service

A C++ REST API service that fetches real-time global flight data
from the OpenSky Network and exposes it through a clean HTTP interface.

Built to demonstrate: C++ OOP design, RESTful API consumption and
production, Service-Oriented Architecture, and CMake-based DevOps.

## Live Demo
https://flight-data-service.onrender.com/api/flights/stats

## Architecture

OpenSky Network API
       |
       v
  [APIClient]        ← Consumes external REST API (libcurl)
       |
       v
  [DataService]      ← Business logic, caching, filtering (SOA)
       |
       v
  [RestServer]       ← Produces REST API (cpp-httplib)
       |
       v
  HTTP Clients

## API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | /health | Service health check |
| GET | /api/flights | All live flights |
| GET | /api/flights/airborne | Airborne flights only |
| GET | /api/flights/stats | Aggregate statistics |
| GET | /api/airports | Monitored airports |
| GET | /api/flights/country?name=India | Filter by country |

## Tech Stack
- Language: C++17
- Build: CMake 3.15+
- HTTP Client: libcurl
- HTTP Server: cpp-httplib
- JSON: nlohmann/json
- Data Source: OpenSky Network API (free, no auth)
- Deployment: Docker + Render

## Build & Run

### Prerequisites
- g++ (C++17)
- CMake 3.15+
- libcurl-dev

### Linux / WSL
    sudo apt install g++ cmake libcurl4-openssl-dev
    git clone https://github.com/RohanBabhale1/flight-data-service.git
    cd flight-data-service
    mkdir build && cd build
    cmake ..
    make -j4
    ./flight_service

## OOP Design

| Class | Responsibility |
|-------|----------------|
| Flight | Encapsulates aircraft state — position, velocity, altitude |
| Airport | Models airports with Haversine-based proximity detection |
| APIClient | HTTP client wrapper — libcurl abstraction |
| DataService | Business logic layer — fetch, parse, filter, cache |
| RestServer | HTTP server — route registration and response handling |

## Author
Babhale Rohan Laxmikant
github.com/RohanBabhale1