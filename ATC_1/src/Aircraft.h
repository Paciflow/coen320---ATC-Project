#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <cstdint>
#include <cmath>
#include <vector>
#include <string>
#include <thread> // Include for std::thread

// Structure to represent the position of an aircraft
struct Position {
    double x;
    double y;
    double z;
};

// Structure to represent the velocity of an aircraft
struct Velocity {
    double speed_x;
    double speed_y;
    double speed_z;
};

// Structure to represent an aircraft
struct Aircraft {
    uint32_t id;
    Position position;
    Velocity velocity;
    double entry_time; // Time when the aircraft enters the airspace

    // Function to update the aircraft's position based on elapsed time
    void updatePosition(double time_elapsed) {
        position.x += velocity.speed_x * time_elapsed;
        position.y += velocity.speed_y * time_elapsed;
        position.z += velocity.speed_z * time_elapsed;
    }

    // Function to provide aircraft information for radar requests
    void getInfo(uint32_t* out_id, Position* out_position, Velocity* out_velocity) const {
        if (out_id) *out_id = id;
        if (out_position) *out_position = position;
        if (out_velocity) *out_velocity = velocity;
    }
};

// Structure to hold aircraft data and thread
struct AircraftThread {
    Aircraft aircraft;
    std::thread thread;
};

// Function that will be executed by each aircraft's thread
void aircraftThreadFunction(Aircraft aircraft);

// Function to create a new aircraft and start its thread
AircraftThread* createAircraftThread(uint32_t id, double x, double y, double z, double speed_x, double speed_y, double speed_z, double entry_time);

#endif // AIRCRAFT_H
