#include "Aircraft.h"
#include <iostream>
#include <chrono>

void aircraftThreadFunction(Aircraft aircraft) {
    while (true) {
        aircraft.updatePosition(1.0); // Update position every second

        // Simulate radar request handling (needs IPC implementation)
        uint32_t id;
        Position pos;
        Velocity vel;
        aircraft.getInfo(&id, &pos, &vel);
        // std::cout << "Aircraft " << id << " at (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
        // In a real system, you'd have a condition to exit the loop/thread
    }
}

AircraftThread* createAircraftThread(uint32_t id, double x, double y, double z, double speed_x, double speed_y, double speed_z, double entry_time) {
    AircraftThread* aircraftThread = new AircraftThread;
    aircraftThread->aircraft.id = id;
    aircraftThread->aircraft.position = {x, y, z};
    aircraftThread->aircraft.velocity = {speed_x, speed_y, speed_z};
    aircraftThread->aircraft.entry_time = entry_time;

    // Launch the thread, passing the Aircraft object by value (it will be copied)
    aircraftThread->thread = std::thread(aircraftThreadFunction, aircraftThread->aircraft);

    return aircraftThread;
}
