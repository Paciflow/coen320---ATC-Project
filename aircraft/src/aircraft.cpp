#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>
#include <thread>
#include <vector>
#include "shared_memory.h"
#include <mutex>

std::mutex cout_mutex;


struct AircraftArgs {
    int index;
    int lifetime;
    SharedMemory* shm;
};

void updateAircraft(AircraftArgs args) {
    int t = 0;

    while (true) {
        args.shm->aircraft[args.index].x += args.shm->aircraft[args.index].speedX;
        args.shm->aircraft[args.index].y += args.shm->aircraft[args.index].speedY;
        args.shm->aircraft[args.index].z += args.shm->aircraft[args.index].speedZ;

        if (t % 2 == 0) {
            auto& a = args.shm->aircraft[args.index];
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << " Aircraft " << a.id
                      << " — Pos(x=" << a.x << ", y=" << a.y << ", z=" << a.z
                      << ") — Speed(vx=" << a.speedX << ", vy=" << a.speedY << ", vz=" << a.speedZ << ")\n";
        }

        sleep(1);
        t++;

        // Optional: deactivate manually after lifetime (just logging now)
        if (t == args.lifetime) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "⚠️  Aircraft " << args.shm->aircraft[args.index].id
                      << " lifetime reached, still flying...\n";
        }

        // If you ever want to let operator deactivate manually:
        // if (args.shm->aircraft[args.index].active == 0) break;

    }
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

    std::ifstream infile("aircraft_input.txt");
    if (!infile.is_open()) {
        infile.open("../aircraft_input.txt");
    }
    if (!infile.is_open()) {
        infile.open("C:/Users/p_uwamuk/ide-7.1-workspace/aircraft/build/aarch64le-debug/aircraft_input.txt");
    }
    if (!infile.is_open()) {
        std::cerr << "❌ aircraft_input.txt not found in any known location.\n";
        return 1;
    }

    if (!infile) {
        std::cerr << "Error: Cannot open aircraft_input.txt\n";
        return 1;
    }

    std::vector<std::thread> threads;
    int id, lifetime;
    float x, y, z, speedX, speedY, speedZ;

    while (infile >> id >> x >> y >> z >> speedX >> speedY >> speedZ >> lifetime) {
        int index = -1;
        for (int i = 0; i < MAX_AIRCRAFT; ++i) {
            if (shm_ptr->aircraft[i].active == 0) {
                shm_ptr->aircraft[i] = {id, x, y, z, speedX, speedY, speedZ, 1};
                index = i;
                shm_ptr->aircraft_count++;
                break;
            }
        }

        if (index != -1) {
        	{
        	    std::lock_guard<std::mutex> lock(cout_mutex);
        	    std::cout << "Aircraft " << id << " entered airspace.\n";
        	}

            AircraftArgs args = {index, lifetime, shm_ptr};
            threads.emplace_back(updateAircraft, args);
        } else {
            std::cerr << "No space left to register aircraft ID: " << id << "\n";
        }
    }

    for (auto& t : threads) {
        t.join();
    }

    munmap(shm_ptr, sizeof(SharedMemory));
    close(shm_fd);
    return 0;
}
