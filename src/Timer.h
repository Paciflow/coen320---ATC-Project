/*
 * Timers.h
 *
 *  Created on: Apr 2, 2025
 *      Author: p_uwamuk
 */

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_



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




#endif /* SRC_TIMER_H_ */
