#include "Display.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>    // For system()
#include <chrono>     // For std::chrono::seconds
#include <thread>     // For std::this_thread::sleep_for>
#include <iomanip>    // For std::setprecision and std::fixed
#include <cstdlib>
// Constructor
Display::Display(const Airspace& airspace, int width, int height) :
        airspace_ref(airspace), display_width(width), display_height(height)
{
    if (display_width <= 10) display_width = 80;
    if (display_height <= 5) display_height = 25;
}

void Display::clearScreen() const {

#ifdef _WIN32
    std::system("cls"); // For Windows
#else
    std::system("clear"); // For Linux, macOS, QNX, and other Unix-like systems
#endif
}

// scalePosition Method
void Display::scalePosition(const Position& real_pos, int& screen_x, int& screen_y) const {
    // TODO: Get boundaries dynamically from Airspace object if they can change.
    const double minX = 0.0;
    const double maxX = 100000.0;
    const double minY = 0.0;
    const double maxY = 100000.0;

    double norm_x = (maxX > minX) ? (real_pos.x - minX) / (maxX - minX) : 0.5;
    double norm_y = (maxY > minY) ? (real_pos.y - minY) / (maxY - minY) : 0.5;

    screen_x = static_cast<int>(norm_x * (display_width - 1));
    screen_y = static_cast<int>((1.0 - norm_y) * (display_height - 1)); // Invert Y-axis

    // Clamp coordinates
    if (screen_x < 0) screen_x = 0;
    if (screen_x >= display_width) screen_x = display_width - 1;
    if (screen_y < 0) screen_y = 0;
    if (screen_y >= display_height) screen_y = display_height - 1;
}

// render Method
void Display::render() const {
    clearScreen();
    std::vector<std::string> grid(display_height, std::string(display_width, ' '));

    // Draw Boundaries
    for (int x = 0; x < display_width; ++x) { grid[0][x] = '-'; grid[display_height - 1][x] = '-'; }
    for (int y = 0; y < display_height; ++y) { grid[y][0] = '|'; grid[y][display_width - 1] = '|'; }
    grid[0][0] = '+'; grid[0][display_width - 1] = '+'; grid[display_height - 1][0] = '+'; grid[display_height - 1][display_width - 1] = '+';

    // WARNING: Direct access to aircraft data is NOT thread-safe without synchronization.
    const std::vector<Aircraft*>& aircraftList = airspace_ref.getAircraft();

    // Draw aircraft symbol and ID
    const char aircraft_char = '@';

    for (const Aircraft* aircraft_ptr : aircraftList) {
        if (aircraft_ptr) {
            int screen_x, screen_y;
            Position current_pos = aircraft_ptr->position; // Unsafe access
            uint32_t id = aircraft_ptr->id;                // Get ID
            scalePosition(current_pos, screen_x, screen_y);

            // Calculate position for the ID character (to the right of the symbol)
            int id_x = screen_x + 1;
            // Get the last digit of the ID (0-9) and convert it to a character ('0'-'9')
            char id_char = (id % 10) + '0';

            // Draw the aircraft symbol '@' if within drawable bounds
            if (screen_y > 0 && screen_y < display_height - 1 &&
                screen_x > 0 && screen_x < display_width - 1)
            {
                grid[screen_y][screen_x] = aircraft_char;
            }

            // Draw the ID character if *its* position is also within drawable bounds
            if (screen_y > 0 && screen_y < display_height - 1 &&
                id_x > 0 && id_x < display_width - 1) // Check id_x here
            {
                // Overwrite whatever might be in the ID's position
                grid[screen_y][id_x] = id_char;
            }
        }
    }

    // Print grid
    for (int y = 0; y < display_height; ++y) {
        std::cout << grid[y] << std::endl;
    }

    std::cout << "Airspace Display | Aircraft Count: " << aircraftList.size() << std::endl;

    // Display detailed aircraft information below the grid
    std::cout << "\n--- Aircraft Details ---" << std::endl;
    std::cout << std::fixed << std::setprecision(2); // For formatting floating-point numbers

    for (const Aircraft* aircraft_ptr : aircraftList) {
        if (aircraft_ptr) {
            uint32_t id = aircraft_ptr->id;
            double altitude_feet = aircraft_ptr->position.z;
            int flight_level = static_cast<int>(altitude_feet / 100.0);
            double speed = std::sqrt(std::pow(aircraft_ptr->velocity.speed_x, 2) +
                                     std::pow(aircraft_ptr->velocity.speed_y, 2) +
                                     std::pow(aircraft_ptr->velocity.speed_z, 2));
            double pos_x = aircraft_ptr->position.x;
            double pos_y = aircraft_ptr->position.y;

            std::cout << "ID: " << id
                      << ", FL: " << flight_level
                      << ", Speed: " << speed
                      << ", Pos: (" << pos_x << ", " << pos_y << ")" << std::endl;
        }
    }
    std::cout << "-------------------------" << std::endl;

    // Add a 5-second delay
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

