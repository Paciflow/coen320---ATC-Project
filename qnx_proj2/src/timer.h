#ifndef TIMER_H_
#define TIMER_H_

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/siginfo.h>
#include <time.h>
#include <unistd.h>

class Timer {
public:
    Timer(int chid);                             // Constructor
    int setTimer(int offset_us, int period_us);  // Set timer (in microseconds)
    ~Timer();                                    // Destructor

private:
    timer_t timerid;             // Timer ID
    struct sigevent event;       // Event (pulse)
    struct itimerspec timer;     // Timer spec
    int coid;                    // Self-connection ID for pulses
};

#endif /* TIMER_H_ */
