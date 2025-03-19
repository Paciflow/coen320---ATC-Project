#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <cstdint>
#include <cmath>

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

// Structure to represent the dimensions (size) of an aircraft
struct Dimensions {
    double length;
    double width;
    double height;
};

// Structure to represent an aircraft
struct Aircraft {
    uint32_t id;
    Position position;
    Velocity velocity;
    Dimensions size;
    double entry_time; // Time when the aircraft enters the airspace
};

// Function to create a new aircraft with size
Aircraft* createAircraft(uint32_t id, double x, double y, double z, double speed_x, double speed_y, double speed_z, double entry_time, double length, double width, double height);

// Function to update the position of an aircraft based on elapsed time
void updateAircraftPosition(Aircraft* aircraft, double time_elapsed);

// Function to get the current information (ID, position, velocity, size) of an aircraft
void getAircraftInfo(const Aircraft* aircraft, uint32_t* id, Position* position, Velocity* velocity, Dimensions* size);

#endif // AIRCRAFT_H
