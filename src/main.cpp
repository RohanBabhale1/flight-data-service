#include <iostream>
#include <memory>
#include <stdexcept>
#include <csignal>
#include "server/RestServer.h"
#include "services/DataService.h"

using namespace FlightDS;

// Global server pointer for signal handling
std::unique_ptr<RestServer> g_server;

// Handle Ctrl+C graceful shutdown
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\n[Main] Shutting down gracefully...\n";
        if (g_server) g_server->stop();
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    // Register signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Parse port from command line argument
    int port = 8080;
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
            if (port < 1024 || port > 65535) {
                std::cerr << "[Main] Invalid port. Using 8080.\n";
                port = 8080;
            }
        } catch (...) {
            std::cerr << "[Main] Invalid port argument. Using 8080.\n";
        }
    }

    std::cout << "[Main] Initializing Flight Data Service...\n";

    try {
        // Create DataService with 60-second cache
        auto dataService = std::make_shared<DataService>(60);

        // Create and start REST server
        g_server = std::make_unique<RestServer>(dataService, port);
        g_server->start();

    } catch (const std::exception& e) {
        std::cerr << "[Main] Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}