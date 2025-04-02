#include <iostream>
#include <vector>
#include <thread>
#include <chrono> // For sleep
#include <csignal> // For signal handling (graceful shutdown)
#include "Aircraft.h"
#include "Airspace.h"
#include "Display.h" // Include the new Display header
#include "Radars.h"


using namespace std;

// --- Global flag for shutdown signal ---
volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signal) {
    stop_flag = 1;
}

int main() {
    // --- Setup Signal Handling ---
    signal(SIGINT, signal_handler); // Handle Ctrl+C
    signal(SIGTERM, signal_handler); // Handle termination signal

    // Create an instance of the Airspace
    Airspace airspace;

    // Create the display, passing it the airspace
    Display display(airspace, 80, 25); // 80 columns, 25 rows

    // Store aircraft threads for later management
    vector<AircraftThread*> aircraft_threads;

    // Create the first aircraft
    AircraftThread* aircraft1 = createAircraftThread(1, 0.0, 0.0, 15000.0, 100.0, 50.0, 0.0, 0.0);
    if (aircraft1) {
        cout << "Created Aircraft 1 with ID: " << aircraft1->aircraft.id << endl;
        airspace.addAircraft(&aircraft1->aircraft);
        aircraft_threads.push_back(aircraft1);
    }

    // Create the second aircraft
    AircraftThread* aircraft2 = createAircraftThread(2, 10000.0, 10000.0, 16000.0, -120.0, -30.0, 0.0, 5.0);
    if (aircraft2) {
        cout << "Created Aircraft 2 with ID: " << aircraft2->aircraft.id << endl;
        airspace.addAircraft(&aircraft2->aircraft);
        aircraft_threads.push_back(aircraft2);
    }

    // Create the third aircraft
    AircraftThread* aircraft3 = createAircraftThread(3, 90000.0, 90000.0, 17000.0, -80.0, -90.0, 0.0, 10.0);
    if (aircraft3) {
        cout << "Created Aircraft 3 with ID: " << aircraft3->aircraft.id << endl;
        airspace.addAircraft(&aircraft3->aircraft);
        aircraft_threads.push_back(aircraft3);
    }

    // --- Initialize and Start Radars ---
       Radars radars;
       if (!radars.initialize()) {
           std::cerr << "Radars initialization failed." << std::endl;
           return 1;
       }
       radars.start(&airspace);
       std::cout << "Radars system started." << std::endl;

       // --- Main Loop ---
       // Run until a stop signal is received (e.g., Ctrl+C)
       // Display updates roughly every second (can adjust sleep duration)
       std::cout << "ATC Simulation running... Press Ctrl+C to stop." << std::endl;
       while (!stop_flag) {
           // Render the airspace using the Display object
           display.render();

           // Sleep for a short duration before the next update
           // Project requires display every 5 seconds, but more frequent updates look better
           // For the project *requirement*, sleep for 5. For testing, 1 is fine.
           std::this_thread::sleep_for(std::chrono::seconds(1)); // Display update interval
       }

       std::cout << "\nShutdown signal received. Stopping aircraft threads..." << std::endl;

       // --- Cleanup ---
       // Stop the radars system
       std::cout << "Stopping radars system..." << std::endl;
       radars.stop();
       std::cout << "Radars system stopped." << std::endl;

    // --- Main Loop ---
    // Run until a stop signal is received (e.g., Ctrl+C)
    // Display updates roughly every second (can adjust sleep duration)
    cout << "ATC Simulation running... Press Ctrl+C to stop." << endl;
    while (!stop_flag) {
        // Render the airspace using the Display object
        display.render();

        // Sleep for a short duration before the next update
        // Project requires display every 5 seconds, but more frequent updates look better
        // For the project *requirement*, sleep for 5. For testing, 1 is fine.
        this_thread::sleep_for(chrono::seconds(1)); // Display update interval
    }

    cout << "\nShutdown signal received. Stopping aircraft threads..." << endl;

    // --- Cleanup ---
    // Signal all aircraft threads to stop
    for (AircraftThread* at : aircraft_threads) {
        if (at) {
            at->should_stop = true;
        }
    }

    // Join all aircraft threads and free memory
    for (AircraftThread* at : aircraft_threads) {
        if (at && at->thread.joinable()) {
            at->thread.join();
        }
        delete at; // Free the allocated AircraftThread struct
    }
    aircraft_threads.clear(); // Clear the vector

    cout << "All aircraft threads stopped and cleaned up." << endl;

    return 0;
}
