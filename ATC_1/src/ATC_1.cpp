#include <iostream>
#include <vector>
#include <thread>
#include "Aircraft.h" // Assuming Aircraft.h is in the same directory
#include "Airspace.h" // Assuming Airspace.h is in the same directory

using namespace std;

int main() {
    // Create an instance of the Airspace
    Airspace airspace;

    // Create the first aircraft
    AircraftThread* aircraft1 = createAircraftThread(1, 0.0, 0.0, 15000.0, 100.0, 50.0, 0.0, 0.0);
    cout << "Created Aircraft 1 with ID: " << aircraft1->aircraft.id << endl;
    airspace.addAircraft(&aircraft1->aircraft); // Add to airspace (passing pointer to the Aircraft object)

    // Create the second aircraft
    AircraftThread* aircraft2 = createAircraftThread(2, 10000.0, 10000.0, 16000.0, 120.0, -30.0, 0.0, 5.0);
    cout << "Created Aircraft 2 with ID: " << aircraft2->aircraft.id << endl;
    airspace.addAircraft(&aircraft2->aircraft); // Add to airspace

    // Create the third aircraft
    AircraftThread* aircraft3 = createAircraftThread(3, 50000.0, 75000.0, 17000.0, 80.0, 90.0, 0.0, 10.0);
    cout << "Created Aircraft 3 with ID: " << aircraft3->aircraft.id << endl;
    airspace.addAircraft(&aircraft3->aircraft); // Add to airspace

    // Keep the main thread alive so the aircraft threads can continue running
    this_thread::sleep_for(chrono::seconds(30)); // Let them run for 30 seconds

    // Clean up: Join the threads and free the allocated memory
    if (aircraft1) { aircraft1->thread.join(); delete aircraft1; }
    if (aircraft2) { aircraft2->thread.join(); delete aircraft2; }
    if (aircraft3) { aircraft3->thread.join(); delete aircraft3; }

    cout << "All aircraft threads finished." << endl;

    return 0;
}
