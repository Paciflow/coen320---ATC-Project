#include "VisualRepresentation.h"
#include <iostream>
#include <iomanip> // For formatting output

void VisualRepresentation::displayPlane(const Aircraft* aircraft) {
    if (aircraft != nullptr) {
        std::cout << "------------------------------" << std::endl;
        std::cout << "      Aircraft Details      " << std::endl;
        std::cout << "------------------------------" << std::endl;
        std::cout << "ID:          " << aircraft->id << std::endl;
        std::cout << "Position:    ("
                  << std::fixed << std::setprecision(2) << aircraft->position.x << ", "
                  << std::fixed << std::setprecision(2) << aircraft->position.y << ", "
                  << std::fixed << std::setprecision(2) << aircraft->position.z << ")" << std::endl;
        std::cout << "Velocity:    ("
                  << std::fixed << std::setprecision(2) << aircraft->velocity.speed_x << ", "
                  << std::fixed << std::setprecision(2) << aircraft->velocity.speed_y << ", "
                  << std::fixed << std::setprecision(2) << aircraft->velocity.speed_z << ")" << std::endl;
        std::cout << "Size (LWH):  ("
                  << std::fixed << std::setprecision(2) << aircraft->size.length << ", "
                  << std::fixed << std::setprecision(2) << aircraft->size.width << ", "
                  << std::fixed << std::setprecision(2) << aircraft->size.height << ")" << std::endl;
        std::cout << "Entry Time:  " << std::fixed << std::setprecision(2) << aircraft->entry_time << std::endl;
        std::cout << "------------------------------" << std::endl;
    } else {
        std::cout << "No aircraft data to display." << std::endl;
    }
}
