cmake_minimum_required(VERSION 3.15)
project(FlightDataService VERSION 1.0.0 LANGUAGES CXX)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
 
include_directories(include)
include_directories(third_party)
include_directories(.)
 
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
 
set(SOURCES
    src/main.cpp
    src/models/Flight.cpp
    src/models/Airport.cpp
    src/services/APIClient.cpp
    src/services/DataService.cpp
    src/server/RestServer.cpp
)
 
add_executable(flight_service ${SOURCES})
 
target_link_libraries(flight_service
    CURL::libcurl
    Threads::Threads
)
 
target_compile_options(flight_service PRIVATE -Wall -Wextra -O2)
 
message(STATUS "CMake configuration complete. Run: make")
