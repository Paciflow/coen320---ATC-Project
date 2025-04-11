# Air Traffic Control (ATC) System - COEN 320 Final Project

## Overview
This project simulates a simplified Air Traffic Control (ATC) system using C++ and QNX. It models aircraft movement, real-time radar display, operator commands, and collision detection using threads and inter-process communication (IPC) via shared memory.

Developed independently as part of the COEN 320 Real-Time Systems course at Concordia University, this project demonstrates core real-time programming concepts including multithreading, inter-process communication, synchronization using mutexes, and modular embedded system design. It also showcases a strong understanding of system-level programming and simulation under constrained operating system environments like QNX.

## Features

- Real-time aircraft simulation using threads
- Shared memory communication across separate processes
- Synchronization of shared data using mutexes
- Radar display with ASCII grid visualization
- Collision detection and prediction alerts
- Operator console for live control (change speed/altitude, add aircraft)
- Modular architecture (separate C++ programs for each component)

## Components

- **aircraft**: Reads input file and launches a thread per aircraft to simulate motion
- **radar_display**: Displays a 2D radar grid with aircraft positions and logs radar alerts
- **computer_system**: Predicts possible collisions and logs warnings
- **operator_console**: CLI interface to modify aircraft properties in real-time
- **shared_memory.h**: Defines the shared structure accessible by all modules

## How It Works

1. Aircraft threads continuously update their positions based on speed and lifetime.
2. Shared memory is used for all modules to access and modify aircraft data.
3. The radar display maps aircraft positions into a 40x20 grid and logs primary/secondary radar info.
4. The computer system calculates proximity between aircraft and predicts collisions.
5. The operator console lets a user change speed or altitude, or add a new aircraft on the fly.
6. A local mutex is used to synchronize output from aircraft threads, preventing race conditions and ensuring clean console output.

## Sample Operator Commands

```
change_speed <id> <vx> <vy> <vz>
change_altitude <id> <new_z>
add_aircraft <id> <x> <y> <z> <vx> <vy> <vz> <lifetime>
```

## Demo Instructions

1. Start the QNX target and launch the shared memory module.
2. Run the `aircraft` process with an input file (e.g., `aircraft_input.txt`).
3. Run `radar_display` to show the grid and logs.
4. Run `computer_system` for collision predictions.
5. Run `operator_console` to interact with live aircraft.

## Tools & Technologies

- C++ (with STL threading)
- QNX Neutrino RTOS
- `shm_open`, `mmap` for IPC
- `std::mutex` for synchronization
- Terminal-based ASCII UI
- GitHub for version control

## Author

Developed independently by:

- **Pacifique Uwamukiza**
- **Aboubacry Ba**
- **Amira Toslim**
  Computer Engineering Students, Concordia University  
  [GitHub: Paciflow](https://github.com/Paciflow)

## Status

Project completed and successfully demoed on April 10, 2025.

---

> This project highlights my ability to design and implement a multithreaded real-time system, use inter-process communication effectively, synchronize shared resources using mutexes, and build a modular architecture in a resource-constrained OS environment. It reflects my technical versatility and dedication to building robust, maintainable systems under academic and technical constraints.

