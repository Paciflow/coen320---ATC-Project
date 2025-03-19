#include "Radar.h"
#include <cstdio>
#include <vector>

void initializeRadar() {
    std::printf("Radar system initialized.\n");
    // Add any radar initialization logic here
}

void scanAirspace(const Airspace* airspace) {
    if (airspace != nullptr) {
        const std::vector<Aircraft*>& aircraft_list = getAircraftInAirspace(airspace);

        std::printf("Radar scanning airspace...\n");
        for (const auto& aircraft : aircraft_list) {
            if (aircraft != nullptr) {
                uint32_t id;
                Position position;
                Velocity velocity;
                Dimensions size;
                getAircraftInfo(aircraft, &id, &position, &velocity, &size);
                std::printf("  Detected Aircraft ID: %u, Position: (%.2f, %.2f, %.2f), Velocity: (%.2f, %.2f, %.2f), Size: (L: %.2f, W: %.2f, H: %.2f)\n",
                       id, position.x, position.y, position.z, velocity.speed_x, velocity.speed_y, velocity.speed_z, size.length, size.width, size.height);
            }
        }
        if (aircraft_list.empty()) {
            std::printf("  No aircraft detected in the airspace.\n");
        }
    } else {
        std::printf("Radar: Airspace is not initialized.\n");
    }
}
