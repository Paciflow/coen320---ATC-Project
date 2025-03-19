#ifndef AIRSPACE_H
#define AIRSPACE_H

#include "Aircraft.h"
#include <cstdint>
#include <cmath>
#include <vector>

// Structure to represent the dimensions of the airspace
struct AirspaceDimensions {
    double length; // Along X-axis (100000 units)
    double width;  // Along Y-axis (100000 units)
    double height; // Along Z-axis (25000 units, from 15000ft above sea level, so maybe 15000 to 40000)
    double min_altitude; // Minimum altitude of the airspace
};

// Structure to represent the airspace
struct Airspace {
    AirspaceDimensions dimensions;
    std::vector<Aircraft*> aircraft_list; // Use a vector for dynamic resizing
    int capacity;             // Maximum number of aircraft the airspace can hold (optional with vector)
};

// Function to create a new airspace
Airspace* createAirspace(double length, double width, double height, double min_altitude, int capacity);

// Function to add an aircraft to the airspace
bool addAircraftToAirspace(Airspace* airspace, Aircraft* aircraft);

// Function to remove an aircraft from the airspace (by ID)
bool removeAircraftFromAirspace(Airspace* airspace, uint32_t aircraft_id);

// Function to get a list of all aircraft currently in the airspace
const std::vector<Aircraft*>& getAircraftInAirspace(const Airspace* airspace);

// Function to check for separation violations between aircraft in the airspace
void checkSeparationViolations(const Airspace* airspace, double current_time_plus_n);

#endif // AIRSPACE_H
