#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>
#include <thread>
#include "shared_memory.h"

struct AircraftArgs {
    int index;
    int lifetime;
    SharedMemory* shm;
};

void updateAircraft(AircraftArgs args) {
    for (int t = 0; t < args.lifetime; ++t) {
        args.shm->aircraft[args.index].x += args.shm->aircraft[args.index].speedX;
        args.shm->aircraft[args.index].y += args.shm->aircraft[args.index].speedY;
        args.shm->aircraft[args.index].z += args.shm->aircraft[args.index].speedZ;
        sleep(1);
    }
    args.shm->aircraft[args.index].active = 0;
    args.shm->aircraft_count--;
    std::cout << "✈️  Aircraft " << args.shm->aircraft[args.index].id << " exited airspace.\n";
}

void printHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  change_speed <id> <vx> <vy> <vz>\n";
    std::cout << "  change_altitude <id> <new_z>\n";
    std::cout << "  add_aircraft <id> <x> <y> <z> <vx> <vy> <vz> <lifetime>\n";
    std::cout << "  show\n";
    std::cout << "  help\n";
    std::cout << "  exit\n";
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

    std::cout << "Operator Console started. Type 'help' for commands.\n";

    std::string line;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "change_speed") {
            int id;
            float vx, vy, vz;
            ss >> id >> vx >> vy >> vz;
            bool found = false;

            for (int i = 0; i < MAX_AIRCRAFT; ++i) {
                if (shm_ptr->aircraft[i].active && shm_ptr->aircraft[i].id == id) {
                    shm_ptr->aircraft[i].speedX = vx;
                    shm_ptr->aircraft[i].speedY = vy;
                    shm_ptr->aircraft[i].speedZ = vz;
                    std::cout << "✔️ Speed updated for Aircraft " << id << "\n";
                    found = true;
                    break;
                }
            }

            if (!found) std::cout << "❌ Aircraft " << id << " not found.\n";
        }
        else if (cmd == "change_altitude") {
            int id;
            float new_z;
            ss >> id >> new_z;
            bool found = false;

            for (int i = 0; i < MAX_AIRCRAFT; ++i) {
                if (shm_ptr->aircraft[i].active && shm_ptr->aircraft[i].id == id) {
                    shm_ptr->aircraft[i].z = new_z;
                    std::cout << "✔️ Altitude updated for Aircraft " << id << "\n";
                    found = true;
                    break;
                }
            }

            if (!found) std::cout << "❌ Aircraft " << id << " not found.\n";
        }
        else if (cmd == "add_aircraft") {
            int id, lifetime;
            float x, y, z, vx, vy, vz;
            ss >> id >> x >> y >> z >> vx >> vy >> vz >> lifetime;

            bool inserted = false;
            for (int i = 0; i < MAX_AIRCRAFT; ++i) {
                if (shm_ptr->aircraft[i].active == 0) {
                    shm_ptr->aircraft[i] = {id, x, y, z, vx, vy, vz, 1};
                    shm_ptr->aircraft_count++;
                    AircraftArgs args = {i, lifetime, shm_ptr};
                    std::thread(updateAircraft, args).detach();
                    std::cout << "✅ Aircraft " << id << " added and flying.\n";
                    inserted = true;
                    break;
                }
            }

            if (!inserted)
                std::cout << "❌ No room to add aircraft. Max capacity reached.\n";
        }
        else if (cmd == "show") {
            for (int i = 0; i < MAX_AIRCRAFT; ++i) {
                if (shm_ptr->aircraft[i].active) {
                    auto& a = shm_ptr->aircraft[i];
                    std::cout << "Aircraft " << a.id << " — Pos(x=" << a.x << ", y=" << a.y << ", z=" << a.z
                              << ") — Speed(vx=" << a.speedX << ", vy=" << a.speedY << ", vz=" << a.speedZ << ")\n";
                }
            }
        }
        else if (cmd == "help") {
            printHelp();
        }
        else if (cmd == "exit") {
            break;
        }
        else {
            std::cout << "❓ Unknown command. Type 'help'.\n";
        }
    }

    munmap(shm_ptr, sizeof(SharedMemory));
    close(shm_fd);
    return 0;
}
