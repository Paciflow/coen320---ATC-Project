#include "Aircraft.h"
#include <cstdlib>
#include <cstdio>

Aircraft* createAircraft(uint32_t id, double x, double y, double z, double speed_x, double speed_y, double speed_z, double entry_time, double length, double width, double height) {
    Aircraft* newAircraft = static_cast<Aircraft*>(std::malloc(sizeof(Aircraft)));
    if (newAircraft == nullptr) {
        std::perror("Failed to allocate memory for aircraft");
        return nullptr;
    }
    newAircraft->id = id;
    newAircraft->position.x = x;
    newAircraft->position.y = y;
    newAircraft->position.z = z;
    newAircraft->velocity.speed_x = speed_x;
    newAircraft->velocity.speed_y = speed_y;
    newAircraft->velocity.speed_z = speed_z;
    newAircraft->entry_time = entry_time;
    newAircraft->size.length = length;
    newAircraft->size.width = width;
    newAircraft->size.height = height;
    return newAircraft;
}

void updateAircraftPosition(Aircraft* aircraft, double time_elapsed) {
    if (aircraft != nullptr) {
        aircraft->position.x += aircraft->velocity.speed_x * time_elapsed;
        aircraft->position.y += aircraft->velocity.speed_y * time_elapsed;
        aircraft->position.z += aircraft->velocity.speed_z * time_elapsed;
    }
}

void getAircraftInfo(const Aircraft* aircraft, uint32_t* id, Position* position, Velocity* velocity, Dimensions* size) {
    if (aircraft != nullptr) {
        if (id != nullptr) *id = aircraft->id;
        if (position != nullptr) *position = aircraft->position;
        if (velocity != nullptr) *velocity = aircraft->velocity;
        if (size != nullptr) *size = aircraft->size;
    }
}
