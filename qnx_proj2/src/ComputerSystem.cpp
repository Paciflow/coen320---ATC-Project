// ComputerSystem.cpp

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <csignal>
#include <cmath>
#include <sys/neutrino.h>
#include "Timer.h"

#define SHM_NAME "/my_radar_shm"
#define MAX_AIRCRAFT 50

struct Position {
    float x, y, z;
};

struct Velocity {
    float speed_x, speed_y, speed_z;
};

struct RadarData {
    uint32_t aircraftId;
    Position position;
    Velocity velocity;
    bool hasSSRData;
};

struct SharedAirspace {
    RadarData aircrafts[MAX_AIRCRAFT];
    int aircraft_count;
};

struct AlertMessage {
    unsigned int id1;
    unsigned int id2;
    int time_to_violation;
    char alert[100];
};

SharedAirspace* shared_space = nullptr;
int n = 10; // Prediction window (can later be dynamic)
int coid = -1; // IPC Connection ID

// ------------------- Utility Functions -------------------

float horizontal_distance(const Position& p1, const Position& p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

float vertical_distance(const Position& p1, const Position& p2) {
    return std::abs(p1.z - p2.z);
}

bool is_violation(const RadarData& a1, const RadarData& a2, int n) {
    Position f1 = { a1.position.x + a1.velocity.speed_x * n,
                    a1.position.y + a1.velocity.speed_y * n,
                    a1.position.z + a1.velocity.speed_z * n };

    Position f2 = { a2.position.x + a2.velocity.speed_x * n,
                    a2.position.y + a2.velocity.speed_y * n,
                    a2.position.z + a2.velocity.speed_z * n };

    return (vertical_distance(f1, f2) < 1000) && (horizontal_distance(f1, f2) < 3000);
}

void send_alert(const RadarData& a1, const RadarData& a2) {
    if (coid == -1) return;

    AlertMessage msg;
    msg.id1 = a1.aircraftId;
    msg.id2 = a2.aircraftId;
    msg.time_to_violation = n;
    strcpy(msg.alert, "Collision risk detected!");

    int status = MsgSend(coid, &msg, sizeof(msg), NULL, 0);
    if (status == -1) {
        perror("MsgSend failed");
    }
    else {
        std::cout << "[ComputerSystem] ALERT sent to Operator Console.\n";
    }
}

// ------------------- Main -------------------

int main() {
    std::cout << "[ComputerSystem] Starting...\n";

    // Connect to shared memory from radar
    int fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (fd == -1) { perror("shm_open"); return 1; }

    shared_space = (SharedAirspace*)mmap(NULL, sizeof(SharedAirspace), PROT_READ, MAP_SHARED, fd, 0);
    if (shared_space == MAP_FAILED) { perror("mmap"); return 1; }

    // Connect to Operator Console IPC
    coid = name_open("operator_console", 0);
    if (coid == -1) { perror("name_open failed"); return 1; }
    std::cout << "[ComputerSystem] Connected to Operator Console.\n";

    // Create channel for timer
    int chid = ChannelCreate(0);
    if (chid == -1) { perror("ChannelCreate"); return 1; }

    // Setup timer (periodic every 1 second)
    Timer timer(chid);
    timer.setTimer(1'000'000, 1'000'000); // 1 second period

    std::cout << "[ComputerSystem] Timer started (1 second period).\n";

    // Message receive buffer
    struct _pulse pulse;

    while (true) {
        int rcvid = MsgReceive(chid, &pulse, sizeof(pulse), NULL);

        if (rcvid == 0 && pulse.code == _PULSE_CODE_MINAVAIL) {
            // Timer triggered

            std::cout << "[ComputerSystem] Checking Airspace...\n";

            for (int i = 0; i < shared_space->aircraft_count; ++i) {
                for (int j = i + 1; j < shared_space->aircraft_count; ++j) {
                    const RadarData& a1 = shared_space->aircrafts[i];
                    const RadarData& a2 = shared_space->aircrafts[j];

                    if (is_violation(a1, a2, n)) {
                        send_alert(a1, a2);
                    }
                }
            }
        }
    }

    // Cleanup (not reached, but good practice)
    munmap(shared_space, sizeof(SharedAirspace));
    close(fd);
    ChannelDestroy(chid);
    name_close(coid);

    return 0;
}
