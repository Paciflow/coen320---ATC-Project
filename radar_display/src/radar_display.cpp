#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>
#include <ctime>
#include "shared_memory.h"

// Grid display size
const int WIDTH = 40;
const int HEIGHT = 20;

// Converts world coordinates to grid positions
int mapToGridX(float x) {
    return std::min(WIDTH - 1, std::max(0, static_cast<int>((x / 10000.0f) * WIDTH)));
}

int mapToGridY(float y) {
    return std::min(HEIGHT - 1, std::max(0, static_cast<int>((y / 10000.0f) * HEIGHT)));
}

// Returns a timestamp string
std::string getTimestamp() {
    time_t now = time(0);
    char buf[80];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now));
    return std::string(buf);
}

// Draws aircraft on a 2D grid using 'O' markers
void drawRadarGrid(SharedMemory* shm_ptr) {
    char display[HEIGHT][WIDTH];

    // Fill grid with dots
    for (int i = 0; i < HEIGHT; ++i)
        for (int j = 0; j < WIDTH; ++j)
            display[i][j] = '.';

    // Place aircraft
    for (int i = 0; i < MAX_AIRCRAFT; ++i) {
        if (shm_ptr->aircraft[i].active) {
            int gx = mapToGridX(shm_ptr->aircraft[i].x);
            int gy = mapToGridY(shm_ptr->aircraft[i].y);
            display[gy][gx] = 'O';
        }
    }

    // Print the grid
    std::cout << "=============== 2D DISPLAY DATA ===============\n";
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            std::cout << display[i][j];
        }
        std::cout << "\n";
    }
    std::cout << "================================================\n";
}

// Print primary/secondary radar logs + warnings
void drawRadarAlerts(SharedMemory* shm_ptr) {
    for (int i = 0; i < MAX_AIRCRAFT; ++i) {
        if (shm_ptr->aircraft[i].active) {
            auto& a = shm_ptr->aircraft[i];

            // Primary radar range check
            if (a.x >= 0 && a.x <= 10000 &&
                a.y >= 0 && a.y <= 10000 &&
                a.z >= 0 && a.z <= 20000) {
                std::cout << "[Radar Primary] Aircraft in bounds: Aircraft ID " << a.id
                          << " at position (" << (int)a.x << ", " << (int)a.y << ", " << (int)a.z << ")\n";
            }

            // Always show interrogation
            std::cout << "[Radar Secondary] Interrogating Aircraft ID " << a.id
                      << " at position (" << (int)a.x << ", " << (int)a.y << ", " << (int)a.z << ")\n";

            // High altitude alert
            if (a.z > 30000) {
                std::cout << "[⚠️ Warning] Aircraft ID " << a.id
                          << " flying at high altitude (" << (int)a.z << ")\n";
            }
        }
    }
}

int main() {
    const char* shm_name = "/atc_shm";
    int shm_fd = shm_open(shm_name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    SharedMemory* shm_ptr = (SharedMemory*) mmap(0, sizeof(SharedMemory),
                                                  PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    std::cout << "Radar Display started...\n";

    while (true) {
        std::cout << "\n========== RADAR DISPLAY ==========\n";
        std::cout << "Time: " << getTimestamp() << "\n\n";

        drawRadarGrid(shm_ptr);
        drawRadarAlerts(shm_ptr);

        sleep(5);
    }

    munmap(shm_ptr, sizeof(SharedMemory));
    close(shm_fd);
    return 0;
}
