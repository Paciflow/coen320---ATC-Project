// shared_memory.h

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define MAX_AIRCRAFT 100

// Top-level Aircraft struct
struct AircraftData {
    int id;
    float x, y, z;
    float speedX, speedY, speedZ;
    int active;
};

// Shared Memory struct
struct SharedMemory {
    AircraftData aircraft[MAX_AIRCRAFT];
    int aircraft_count;
};

#endif
