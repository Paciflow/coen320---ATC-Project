// project.cpp

#include "Aircraft.h"
#include "Airspace.h"
#include "Radar.h"
#include "VisualRepresentation.h" // Include the displaying class
#include <iostream>
#include <unistd.h> // For sleep (simulating time passing)

int main() {
    std::cout << "Simplified ATC System Starting..." << std::endl;

    // Create an airspace
    Airspace* airspace = createAirspace(100000.0, 100000.0, 25000.0, 15000.0, 10);
    if (airspace == nullptr) {
        std::cerr << "Failed to create airspace." << std::endl;
        return 1;
    }
    std::cout << "Airspace created." << std::endl;

    // Create some aircraft
    Aircraft* aircraft1 = createAircraft(1, 10000.0, 10000.0, 20000.0, 100.0, 0.0, 0.0, 0.0, 50.0, 50.0, 10.0);
    Aircraft* aircraft2 = createAircraft(2, 15000.0, 10000.0, 20500.0, 100.0, 0.0, 0.0, 1.0, 60.0, 45.0, 12.0);
    if (aircraft1 == nullptr || aircraft2 == nullptr) {
        std::cerr << "Failed to create aircraft." << std::endl;
        // Clean up allocated memory
        if (airspace) {
            free(airspace);
        }
        if (aircraft1) {
            free(aircraft1);
        }
        if (aircraft2) {
            free(aircraft2);
        }
        return 1;
    }
    std::cout << "Aircraft created." << std::endl;

    // Display initial aircraft details
    std::cout << "\nInitial Aircraft Details:" << std::endl;
    VisualRepresentation::displayPlane(aircraft1);
    VisualRepresentation::displayPlane(aircraft2);

    // Add aircraft to the airspace
    if (addAircraftToAirspace(airspace, aircraft1)) {
        std::cout << "Aircraft 1 added to airspace." << std::endl;
    } else {
        std::cerr << "Failed to add Aircraft 1 to airspace." << std::endl;
    }

    if (addAircraftToAirspace(airspace, aircraft2)) {
        std::cout << "Aircraft 2 added to airspace." << std::endl;
    } else {
        std::cerr << "Failed to add Aircraft 2 to airspace." << std::endl;
    }

    // Initialize radar
    initializeRadar();

    // Simulate radar scanning the airspace
    scanAirspace(airspace);

    // Simulate some time passing and update aircraft positions significantly
    std::cout << "\nSimulating time passing..." << std::endl;
    sleep(5); // Wait for 5 seconds (simulating time)
    updateAircraftPosition(aircraft1, 600.0); // Move for 600 seconds (10 minutes)
    updateAircraftPosition(aircraft2, 600.0); // Move for 600 seconds (10 minutes)
    std::cout << "Aircraft positions updated." << std::endl;

    // Display aircraft details after movement
    std::cout << "\nAircraft Details After Movement:" << std::endl;
    VisualRepresentation::displayPlane(aircraft1);
    VisualRepresentation::displayPlane(aircraft2);

    // Check for separation violations after a longer time horizon
    std::cout << "\nChecking for separation violations..." << std::endl;
    checkSeparationViolations(airspace, 300.0); // Check for violations within the next 300 seconds (5 minutes)

    // Simulate more time passing and update positions again
    std::cout << "\nSimulating more time passing..." << std::endl;
    sleep(10); // Wait for 10 seconds
    updateAircraftPosition(aircraft1, 1200.0); // Move for another 1200 seconds (20 minutes)
    updateAircraftPosition(aircraft2, 1200.0); // Move for another 1200 seconds (20 minutes)
    std::cout << "Aircraft positions updated again." << std::endl;

    // Display aircraft details after more movement
    std::cout << "\nAircraft Details After More Movement:" << std::endl;
    VisualRepresentation::displayPlane(aircraft1);
    VisualRepresentation::displayPlane(aircraft2);

    // Check for separation violations again
    std::cout << "\nChecking for separation violations again..." << std::endl;
    checkSeparationViolations(airspace, 600.0); // Check for violations within the next 600 seconds (10 minutes)

    // Clean up allocated memory
    free(aircraft1);
    free(aircraft2);
    free(airspace);

    std::cout << "\nSimplified ATC System Finished." << std::endl;

    return 0;
}
