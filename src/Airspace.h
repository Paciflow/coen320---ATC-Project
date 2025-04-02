/*
 * Airspace.h
 *
 *  Created on: Apr 2, 2025
 *      Author: p_uwamuk
 */


#ifndef AIRSPACE_H
#define AIRSPACE_H

#include "Aircraft.h" // Assuming Aircraft.h exists
#include <vector>

class Airspace {
public:
    // Constructor to initialize the airspace boundaries
    Airspace();

    // Method to check if a given position is within the airspace
    bool isWithinBounds(const Position& pos) const;

    // Method to check if a given aircraft is within the airspace
    bool isWithinBounds(const Aircraft& aircraft) const;

    // Method to add an aircraft to the airspace
    void addAircraft(Aircraft* aircraft);

    // Method to remove an aircraft from the airspace
    void removeAircraft(uint32_t aircraftId);

    // Method to get all aircraft currently in the airspace
    const std::vector<Aircraft*>& getAircraft() const;

private:
    // Boundaries of the airspace
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;

    // Collection to store aircraft in the airspace
    std::vector<Aircraft*> aircraftInSpace;
};

#endif // AIRSPACE_H

