#include <iostream>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>
#include <fstream>
#include <ctime>
#include "shared_memory.h"

#define POLL_INTERVAL_US 100000
#define PREDICT_SECONDS 10
#define MAX_TRACKED 100

bool checkFutureViolation(const AircraftData& a, const AircraftData& b, int n) {
    float ax = a.x + a.speedX * n;
    float ay = a.y + a.speedY * n;
    float az = a.z + a.speedZ * n;

    float bx = b.x + b.speedX * n;
    float by = b.y + b.speedY * n;
    float bz = b.z + b.speedZ * n;

    return (
        std::abs(ax - bx) < 3000 &&
        std::abs(ay - by) < 3000 &&
        std::abs(az - bz) < 1000
    );
}

std::string getTimestamp() {
    time_t now = time(0);
    char buf[80];
    strftime(buf, sizeof(buf), "[%H:%M:%S]", localtime(&now));
    return std::string(buf);
}

int main() {
    const char* shm_name = "/atc_shm";
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    SharedMemory* shm_ptr = (SharedMemory*) mmap(0, sizeof(SharedMemory),
                                                  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    std::cout << "Computer System started. Predicting collisions in "
              << PREDICT_SECONDS << " seconds...\n";

    bool predictionMatrix[MAX_TRACKED][MAX_TRACKED] = {false};

    std::ofstream logfile("/tmp/atc_log.txt", std::ios::app);
    if (!logfile.is_open()) {
        std::cerr << "❌ Failed to open log file at /tmp/atc_log.txt\n";
        return 1;
    }

    while (true) {
        for (int i = 0; i < MAX_AIRCRAFT; ++i) {
            if (!shm_ptr->aircraft[i].active) continue;

            for (int j = i + 1; j < MAX_AIRCRAFT; ++j) {
                if (!shm_ptr->aircraft[j].active) continue;

                bool willCollide = checkFutureViolation(
                    shm_ptr->aircraft[i], shm_ptr->aircraft[j], PREDICT_SECONDS
                );

                if (willCollide && !predictionMatrix[i][j]) {
                    std::string msg = "PREDICTION: Aircraft " +
                                      std::to_string(shm_ptr->aircraft[i].id) + " and " +
                                      std::to_string(shm_ptr->aircraft[j].id) +
                                      " may collide in " + std::to_string(PREDICT_SECONDS) + " seconds!";

                    std::string logEntry = getTimestamp() + " " + msg;

                    std::cout << logEntry << "\n";
                    logfile << logEntry << "\n";
                    logfile.flush();

                    predictionMatrix[i][j] = true;
                }

                if (!willCollide && predictionMatrix[i][j]) {
                    predictionMatrix[i][j] = false;
                }
            }
        }

        usleep(POLL_INTERVAL_US);
    }

    logfile.close();
    munmap(shm_ptr, sizeof(SharedMemory));
    close(shm_fd);
    return 0;
}
