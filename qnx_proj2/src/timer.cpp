// Timer.cpp

#include "Timer.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

// Constructor
Timer::Timer(int chid) {
    // Connect to yourself (the channel)
    coid = ConnectAttach(0, 0, chid, 0, 0);
    if (coid == -1) {
        perror("ConnectAttach failed");
        exit(EXIT_FAILURE);
    }

    // Setup the pulse event
    SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, _PULSE_CODE_MINAVAIL, 0);

    // Create the timer
    if (timer_create(CLOCK_REALTIME, &event, &timerid) == -1) {
        perror("timer_create failed");
        exit(EXIT_FAILURE);
    }
}

// Set Timer
int Timer::setTimer(int offset_us, int period_us) {
    timer.it_value.tv_sec = offset_us / 1'000'000;
    timer.it_value.tv_nsec = (offset_us % 1'000'000) * 1000;
    timer.it_interval.tv_sec = period_us / 1'000'000;
    timer.it_interval.tv_nsec = (period_us % 1'000'000) * 1000;

    return timer_settime(timerid, 0, &timer, nullptr);
}

// Destructor
Timer::~Timer() {
    timer_delete(timerid);
    ConnectDetach(coid);
}
