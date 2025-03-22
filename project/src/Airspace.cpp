#include "Airspace.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>

Airspace* createAirspace(double length, double width, double height, double min_altitude, int capacity) {
    Airspace* newAirspace = static_cast<Airspace*>(std::malloc(sizeof(Airspace)));
    if (newAirspace == nullptr) {
        std::perror("Failed to allocate memory for airspace");
        return nullptr;
    }
    newAirspace->dimensions.length = length;
    newAirspace->dimensions.width = width;
    newAirspace->dimensions.height = height;
    newAirspace->dimensions.min_altitude = min_altitude;
    newAirspace->capacity = capacity;
    newAirspace->aircraft_list.reserve(capacity); // Reserve space for efficiency
    return newAirspace;
}

bool addAircraftToAirspace(Airspace* airspace, Aircraft* aircraft) {
    if (airspace != nullptr && aircraft != nullptr && (airspace->aircraft_list.size() < airspace->capacity)) {
        airspace->aircraft_list.push_back(aircraft);
        return true;
    }
    return false;
}

bool removeAircraftFromAirspace(Airspace* airspace, uint32_t aircraft_id) {
    if (airspace != nullptr) {
        for (auto it = airspace->aircraft_list.begin(); it != airspace->aircraft_list.end(); ++it) {
            if (*it != nullptr && (*it)->id == aircraft_id) {
                airspace->aircraft_list.erase(it);
                return true;
            }
        }
    }
    return false;
}

const std::vector<Aircraft*>& getAircraftInAirspace(const Airspace* airspace) {
    return airspace->aircraft_list;
}

void checkSeparationViolations(const Airspace* airspace, double current_time_plus_n) {
    if (airspace != nullptr && airspace->aircraft_list.size() > 1) {
        size_t num_aircraft = airspace->aircraft_list.size();
        for (size_t i = 0; i < num_aircraft; ++i) {
            for (size_t j = i + 1; j < num_aircraft; ++j) {
                Aircraft* a1 = airspace->aircraft_list[i];
                Aircraft* a2 = airspace->aircraft_list[j];

                if (a1 == nullptr || a2 == nullptr) continue;

                // Predict their positions at current_time + n
                Position pos1 = a1->position;
                Velocity vel1 = a1->velocity;
                Position future_pos1 = {pos1.x + vel1.speed_x * current_time_plus_n,
                                        pos1.y + vel1.speed_y * current_time_plus_n,
                                        pos1.z + vel1.speed_z * current_time_plus_n};

                Position pos2 = a2->position;
                Velocity vel2 = a2->velocity;
                Position future_pos2 = {pos2.x + vel2.speed_x * current_time_plus_n,
                                        pos2.y + vel2.speed_y * current_time_plus_n,
                                        pos2.z + vel2.speed_z * current_time_plus_n};

                // Calculate the distance between the predicted positions
                double dx = future_pos1.x - future_pos2.x;
                double dy = future_pos1.y - future_pos2.y;
                double dz = future_pos1.z - future_pos2.z;
                double horizontal_distance_sq = dx * dx + dy * dy;
                double vertical_distance = std::fabs(dz);
            }
        }
    }
}
