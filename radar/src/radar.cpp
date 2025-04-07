#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include "shared_memory.h"

int main() {
    const char* shm_name = "/atc_shm";
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    ftruncate(shm_fd, sizeof(SharedMemory));

    SharedMemory* shm_ptr = (SharedMemory*) mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    memset(shm_ptr, 0, sizeof(SharedMemory)); // Initialize to 0

    std::cout << "Radar process started. Tracking aircraft...\n";

    // Radar simulation loop
    while (true) {
        for (int i = 0; i < shm_ptr->aircraft_count; ++i) {
            if (shm_ptr->aircraft[i].active) {
                // Simulate position update
                shm_ptr->aircraft[i].x += shm_ptr->aircraft[i].speedX;
                shm_ptr->aircraft[i].y += shm_ptr->aircraft[i].speedY;
                shm_ptr->aircraft[i].z += shm_ptr->aircraft[i].speedZ;
            }
        }

        sleep(1); // Update every second
    }

    munmap(shm_ptr, sizeof(SharedMemory));
    close(shm_fd);
    shm_unlink(shm_name); // Optional: for cleanup

    return 0;
}
