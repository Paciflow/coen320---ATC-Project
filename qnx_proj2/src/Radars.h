/*
 * Radars.h
 *
 *  Created on: Apr. 1, 2025
 *      Author: guidi
 */
#ifndef RADAR_H
#define RADAR_H

#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cmath>
#include <ctime>
#include "Airspace.h"
#include "Aircraft.h"

// Forward declarations of classes used by Radars
class Airspace;
class Aircraft;



// Structure to hold radar data
struct RadarData {
    uint32_t aircraftId;
    Position position;
    Velocity velocity;
    bool hasSSRData; // Flag to indicate if Secondary Surveillance Radar data is valid

    RadarData() : aircraftId(0), position({0.0, 0.0, 0.0}), velocity({0.0, 0.0, 0.0}), hasSSRData(false) {}
};

// Constants

const size_t SHM_SIZE = 4096; // Example size, adjust as needed
const int PRIMARY_RADAR_PULSE_INTERVAL = 5;   // Seconds
const int SECONDARY_RADAR_INTERROGATION_INTERVAL = 2; // Seconds
const int RADAR_SCAN_INTERVAL = 10;         // Seconds
const double RADAR_RANGE = 10000.0;          // Example range in some unit

class Radars {
public:
    Radars();
    ~Radars();

    void start(Airspace* airspace);
    void stop();
    bool initialize();

private:
    bool isRunning;
    Airspace* airspace;
    pthread_t radarThread;
    pthread_mutex_t mutex;
    std::queue<RadarData> dataQueue;

    // Shared memory related members
    int shm_fd;
    void* ptr;

    static void* radarThreadFunc(void* arg);
    void run();
    void performPrimarySurveillance();
    void performSecondarySurveillance();
    void scanAirspace();
    void processDataQueue();
    double calculateDistance(const Position& pos);
};

#endif // RADAR_H
