// Aircraft.cpp

#include "Aircraft.h"
#include <iostream>
#include <chrono>
#include <thread> // Ensure thread is included
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>


// Thread function now takes a pointer to the AircraftThread struct
void aircraftThreadFunction(AircraftThread* data) {
   if (!data) return; // Safety check

   // Loop until the stop flag is set
   while (!data->should_stop) {
       // Access the aircraft data via the pointer
       data->aircraft.updatePosition(1.0); // Update position every second

       // Simulate radar request handling (example, real implementation would use IPC)
       // uint32_t id;
       // Position pos;
       // Velocity vel;
       // data->aircraft.getInfo(&id, &pos, &vel);
       // std::cout << "Aircraft " << data->aircraft.id << " updated position." << std::endl;

       // Sleep for 1 second
       std::this_thread::sleep_for(std::chrono::seconds(1));
   }
   std::cout << "Aircraft " << data->aircraft.id << " thread stopping." << std::endl;
}

AircraftThread* createAircraftThread(uint32_t id, double x, double y, double z, double speed_x, double speed_y, double speed_z, double entry_time) {
   AircraftThread* aircraftThread = new AircraftThread;
   if (!aircraftThread) return nullptr; // Check allocation

   aircraftThread->aircraft.id = id;
   aircraftThread->aircraft.position = {x, y, z};
   aircraftThread->aircraft.velocity = {speed_x, speed_y, speed_z};
   aircraftThread->aircraft.entry_time = entry_time;
   aircraftThread->should_stop = false; // Initialize stop flag

   // Launch the thread, passing the pointer to the struct itself
   try {
       aircraftThread->thread = std::thread(aircraftThreadFunction, aircraftThread);
   } catch (const std::system_error& e) {
       std::cerr << "Failed to create thread for aircraft " << id << ": " << e.what() << std::endl;
       delete aircraftThread;
       return nullptr;
   }


   return aircraftThread;
}
