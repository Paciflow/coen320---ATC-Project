#ifndef RADAR_H
#define RADAR_H

#include "Aircraft.h"
#include "Airspace.h"

// Function to initialize the radar system (if needed)
void initializeRadar();

// Function to simulate the radar scanning the airspace and detecting aircraft
// This function will need to interact with the Airspace to get the list of aircraft.
void scanAirspace(const Airspace* airspace);

#endif // RADAR_H
