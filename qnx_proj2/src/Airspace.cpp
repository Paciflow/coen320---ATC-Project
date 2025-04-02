#include "Airspace.h"

Airspace::Airspace() :
    minX(0.0),
    maxX(100000.0),
    minY(0.0),
    maxY(100000.0),
    minZ(15000.0),
    maxZ(15000.0 + 25000.0)
{
}

bool Airspace::isWithinBounds(const Position& pos) const {
    return (pos.x >= minX && pos.x <= maxX &&
            pos.y >= minY && pos.y <= maxY &&
            pos.z >= minZ && pos.z <= maxZ);
}

bool Airspace::isWithinBounds(const Aircraft& aircraft) const {
    return isWithinBounds(aircraft.position);
}

void Airspace::addAircraft(Aircraft* aircraft) {
    aircraftInSpace.push_back(aircraft);
}

void Airspace::removeAircraft(uint32_t aircraftId) {
    for (auto it = aircraftInSpace.begin(); it != aircraftInSpace.end(); ++it) {
        if ((*it)->id == aircraftId) {
            aircraftInSpace.erase(it);
            return;
        }
    }
}

const std::vector<Aircraft*>& Airspace::getAircraft() const {
    return aircraftInSpace;
}
