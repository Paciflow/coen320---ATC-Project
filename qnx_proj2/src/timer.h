// Timer.h

#ifndef TIMER_H
#define TIMER_H

#include <signal.h>
#include <time.h>
#include <sys/neutrino.h>

class Timer {
public:
    Timer(int chid);
    ~Timer();
    int setTimer(int offset_us, int period_us);

private:
    timer_t timerid;         // Timer ID
    struct sigevent event;   // Event (pulse)
    struct itimerspec timer; // Timer specification
    int coid;                // Connection ID
};

#endif
