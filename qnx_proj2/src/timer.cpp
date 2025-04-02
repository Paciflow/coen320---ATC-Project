#include "Timer.h"

// Constructor
Timer::Timer(int chid) {
    // Connect to yourself
    coid = ConnectAttach(0, 0, chid, 0, 0);
    if (coid == -1) {
        perror("Timer ConnectAttach");
        exit(EXIT_FAILURE);
    }

    // Prepare pulse event
    SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, _PULSE_CODE_MINAVAIL, 0);

    // Create the timer
    if (timer_create(CLOCK_REALTIME, &event, &timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }
}

// Set the timer (offset + period in microseconds)
int Timer::setTimer(int offset_us, int period_us) {
    timer.it_value.tv_sec = offset_us / 1000000;
    timer.it_value.tv_nsec = (offset_us % 1000000) * 1000;
    timer.it_interval.tv_sec = period_us / 1000000;
    timer.it_interval.tv_nsec = (period_us % 1000000) * 1000;

    return timer_settime(timerid, 0, &timer, NULL);
}

// Destructor
Timer::~Timer() {
    timer_delete(timerid);
    ConnectDetach(coid);
}
