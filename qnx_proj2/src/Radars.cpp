
// Radars.cpp
#include "Radars.h"
#include <iostream>

using namespace std;

const char* SHM_NAME = "/my_radar_shm";
Radars::Radars() : isRunning(false), airspace(nullptr), shm_fd(-1), ptr(MAP_FAILED) {
    pthread_mutex_init(&mutex, nullptr);
}

Radars::~Radars() {
    pthread_mutex_destroy(&mutex);
    if (ptr != MAP_FAILED) {
        if (munmap(ptr, SHM_SIZE) == -1) {
            perror("munmap in Radars destructor");
        }
        ptr = MAP_FAILED; // Reset ptr after unmapping
    }
    if (shm_fd != -1) {
        if (close(shm_fd) == -1) {
            perror("close in Radars destructor");
        }
        shm_fd = -1; // Reset shm_fd after closing
    }
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink in Radars destructor");
    }
}

void Radars::start(Airspace* airspace) {
    if (!isRunning) {
        this->airspace = airspace;
        isRunning = true;
        pthread_create(&radarThread, nullptr, radarThreadFunc, this);
    }
}

void Radars::stop() {
    if (isRunning) {
        isRunning = false;
        pthread_join(radarThread, nullptr);
    }
}

bool Radars::initialize() {
    cout << "Radars system initialized." << endl;
    // Open the shared memory segment
    shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd == -1) {
        perror("in shm_open()");
        return false;
    }

    // Configure the size of the shared memory segment
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        close(shm_fd);
        shm_fd = -1;
        return false;
    }

    // Map the shared memory segment in the address space of the process
    ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("in mmap()");
        close(shm_fd);
        shm_fd = -1;
        return false;
    }
    return true;
}

void* Radars::radarThreadFunc(void* arg) {
    static_cast<Radars*>(arg)->run();
    return nullptr; // Corrected return statement
}

void Radars::run() {
    time_t lastPrimaryPulse = 0;
    time_t lastSecondaryInterrogation = 0;
    time_t lastScan = 0;

    while (isRunning) {
        time_t currentTime = time(0);

        if (currentTime - lastPrimaryPulse >= PRIMARY_RADAR_PULSE_INTERVAL) {
            performPrimarySurveillance();
            lastPrimaryPulse = currentTime;
        }

        if (currentTime - lastSecondaryInterrogation >= SECONDARY_RADAR_INTERROGATION_INTERVAL) {
            performSecondarySurveillance();
            lastSecondaryInterrogation = currentTime;
        }

        if (currentTime - lastScan >= RADAR_SCAN_INTERVAL) {
            scanAirspace();
            lastScan = currentTime;
        }

        // Process data queue and send to computer system via shared memory
        processDataQueue();
        sleep(1); // Yield to other threads
    }
}

void Radars::performPrimarySurveillance() {
    cout << "Primary Surveillance Radars pulse emitted." << endl;
    // Simulate primary radar detection with range limitation
    if (airspace != nullptr) {
        const std::vector<Aircraft*>& aircraft_list = airspace->getAircraft();
        for (const auto& aircraft : aircraft_list) {
            if (aircraft != nullptr) {
                Position position = aircraft->position;
                double distance = calculateDistance(position);
                if (distance <= RADAR_RANGE) {
                    cout << "  Primary Radars detected aircraft (ID: " << aircraft->id << ") at distance: " << distance << endl;
                }
            }
        }
    }
}

void Radars::performSecondarySurveillance() {
    cout << "Secondary Surveillance Radars interrogation emitted." << endl;
    // Simulate secondary radar interrogation and response
    if (airspace != nullptr) {
        const vector<Aircraft*>& aircraft_list = airspace->getAircraft();
        for (const auto& aircraft : aircraft_list) {
            if (aircraft != nullptr) {
                uint32_t id;
                Position position;
                Velocity velocity;
                aircraft->getInfo(&id, &position, &velocity);

                // Simulate transponder response
                cout << "  Aircraft ID: " << id << " responded to SSR." << endl;

                // Store data in the queue for processing
                RadarData data;
                data.aircraftId = id;
                data.position = position;
                data.velocity = velocity;
                data.hasSSRData = true; // Indicate valid SSR data
                pthread_mutex_lock(&mutex);
                dataQueue.push(data);
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}

void Radars::scanAirspace() {
    if (airspace != nullptr) {
        const std::vector<Aircraft*>& aircraft_list = airspace->getAircraft();

        std::printf("Radars scanning airspace...\n");
        for (const auto& aircraft : aircraft_list) {
            if (aircraft != nullptr) {
                uint32_t id;
                Position position;
                Velocity velocity;
                aircraft->getInfo(&id, &position, &velocity);
                std::printf("  Detected Aircraft ID: %u, Position: (%.2f, %.2f, %.2f), Velocity: (%.2f, %.2f, %.2f)\n",
                            id, position.x, position.y, position.z, velocity.speed_x, velocity.speed_y, velocity.speed_z);
            }
        }
        if (aircraft_list.empty()) {
            std::printf("  No aircraft detected in the airspace.\n");
        }
    }
    else {
        std::printf("Radars: Airspace is not initialized.\n");
    }
}

void Radars::processDataQueue() {
    pthread_mutex_lock(&mutex);
    while (!dataQueue.empty()) {
        RadarData data = dataQueue.front();
        dataQueue.pop();
        pthread_mutex_unlock(&mutex);

        // Write data to shared memory
        // This is a simplified example, you'll need to manage the shared memory structure
        // more carefully based on your Computer System's expectations.
        memcpy(ptr, &data, sizeof(RadarData));
        cout << "  Sent aircraft data to Computer System (ID: " << data.aircraftId << ")" << endl;
        pthread_mutex_lock(&mutex);
    }
    pthread_mutex_unlock(&mutex);
}

double Radars::calculateDistance(const Position& pos) {
    return sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
}

