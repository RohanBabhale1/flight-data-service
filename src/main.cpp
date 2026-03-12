#include <iostream>
#include <memory>
#include <stdexcept>
#include <csignal>
#include "server/RestServer.h"
#include "services/DataService.h"
 
using namespace std;
using namespace FlightDS;
 
unique_ptr<RestServer> g_server;
 
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        cout << "\n[Main] Shutting down gracefully...\n";
        if (g_server) g_server->stop();
        exit(0);
    }
}
 
int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
 
    int port = 8080;
    if (argc > 1) {
        try {
            port = stoi(argv[1]);
            if (port < 1024 || port > 65535) {
                cerr << "[Main] Invalid port. Using 8080.\n";
                port = 8080;
            }
        } catch (...) {
            cerr << "[Main] Invalid port argument. Using 8080.\n";
        }
    }
 
    cout << "[Main] Initializing Flight Data Service...\n";
 
    try {
        auto dataService = make_shared<DataService>(60);
        g_server = make_unique<RestServer>(dataService, port);
        g_server->start();
    } catch (const exception& e) {
        cerr << "[Main] Fatal error: " << e.what() << "\n";
        return 1;
    }
 
    return 0;
}
