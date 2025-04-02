/*
 * Display.h
 *
 *  Created on: Apr 2, 2025
 *      Author: p_uwamuk
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_



#include "Airspace.h" // Needs access to Airspace
#include <vector>
#include <string>

class Display {
public:
    // Constructor needs a reference to the airspace it will display
    // Also takes the desired width and height of the character display
    Display(const Airspace& airspace, int width = 80, int height = 25);

    // Function to render the current state of the airspace to the console
    void render() const;

    // Function to clear the console screen (platform-dependent)
    void clearScreen() const;

private:
    const Airspace& airspace_ref; // Reference to the airspace data
    int display_width;
    int display_height;

    // Helper to scale real-world coordinates to display coordinates
    void scalePosition(const Position& real_pos, int& screen_x, int& screen_y) const;
};



#endif /* SRC_DISPLAY_H_ */
