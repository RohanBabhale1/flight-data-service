cmake_minimum_required(VERSION 3.15)
project(FlightDataService VERSION 1.0.0 LANGUAGES CXX)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)
include_directories(third_party)
include_directories(.)

# Find required packages
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Collect all source files
set(SOURCES
    src/main.cpp
    src/models/Flight.cpp
    src/models/Airport.cpp
    src/services/APIClient.cpp
    src/services/DataService.cpp
    src/server/RestServer.cpp
)

# Create executable
add_executable(flight_service ${SOURCES})

# Link libraries
target_link_libraries(flight_service
    CURL::libcurl
    Threads::Threads
)

# Compiler flags
target_compile_options(flight_service PRIVATE -Wall -Wextra -O2)

message(STATUS "CMake configuration complete. Run: make")